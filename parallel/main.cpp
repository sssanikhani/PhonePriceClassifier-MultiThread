#include "main.hpp"
#include "utils.hpp"
using namespace std;

#define NUM_THREADS 4
#define NUMBER_OF_CLASSES 4
pthread_mutex_t mutex_extremum;
pthread_mutex_t mutex_main_shared;

const string WEIGHTS_FILE_NAME = "weights.csv";


struct {

    vector<vector<double>> max_col;
    vector<vector<double>> min_col;

} shared_extremums;


struct {

    vector<double> main_max_col;
    vector<double> main_min_col;

} main_shared;


typedef struct
{
    string data_path;
    string weights_path;
} FILE_PATH;


void normilize(vector<vector<double>> &data)
{   
    int data_size = data.size();
    int num_cols = data[0].size() - 1;

    for (int j = 0; j < num_cols; j++)
    {   

        double extremum_diff = main_shared.main_max_col[j] - main_shared.main_min_col[j];
        for (int i = 0; i < data_size; i++)
        {

            data[i][j] = (data[i][j] - main_shared.main_min_col[j]) / extremum_diff;

        }

    }

}


int predict_price_class(const vector<double> &attrs, const vector<vector<double>> &weights)
{   
    int num_cols = weights[0].size();
    double weights_res[NUMBER_OF_CLASSES];

    for (int i = 0; i < NUMBER_OF_CLASSES; i++)
    {
        weights_res[i] = inner_multiply(attrs, weights[i], num_cols - 1);
        weights_res[i] += weights[i][num_cols - 1];
    }

    double max_res = weights_res[0];
    int ind = 0;

    for (int i = 1; i < NUMBER_OF_CLASSES; i++)
    {
        if (weights_res[i] > max_res)
        {
            max_res = weights_res[i];
            ind = i;
        }
    }

    return ind;
}


void* find_extremums(void* args)
{
    long data_size;
    FILE_PATH *file_path_struct = (FILE_PATH*) args;
    string path = file_path_struct->data_path;

    vector<vector<double>> dataset;
    vector<string> headers;
    vector<double> min_col;
    vector<double> max_col;

    read_double_csv_file(path, headers, dataset);
    find_extremum_in_columns(dataset, max_col, min_col);

    pthread_mutex_lock(&mutex_extremum);
    shared_extremums.max_col.push_back(max_col);
    shared_extremums.min_col.push_back(min_col);
    pthread_mutex_unlock(&mutex_extremum);

    data_size = dataset.size();
    pthread_exit((void*)data_size);
}


void* predict_data_file(void* args)
{
    FILE_PATH* file_path_struct = (FILE_PATH*) args;
    string data_path = file_path_struct->data_path;
    string weights_path = file_path_struct->weights_path;

    vector<vector<double>> dataset;
    vector<vector<double>> weights;
    vector<string> headers;

    read_double_csv_file(data_path, headers, dataset);
    read_double_csv_file(weights_path, headers, weights);

    int data_size = dataset.size();
    int goal_column = dataset[0].size() - 1;
    normilize(dataset);

    long true_predictions = 0;
    for (int i = 0; i < data_size; i++)
    {
        int predicted = predict_price_class(dataset[i], weights);
        if (predicted == dataset[i][goal_column])
            true_predictions++;
    }


    pthread_exit((void*)true_predictions);
}


void set_main_extremums()
{
    int col_size = shared_extremums.max_col[0].size();
    for (int j = 0; j < col_size; j++)
    {
        double min_ = shared_extremums.min_col[0][j];
        double max_ = shared_extremums.max_col[0][j];

        for (int i = 1; i < NUM_THREADS; i++)
        {
            if (min_ > shared_extremums.min_col[i][j])
                min_ = shared_extremums.min_col[i][j];
            if (max_ < shared_extremums.max_col[i][j])
                max_ = shared_extremums.max_col[i][j];
        }
        main_shared.main_max_col.push_back(max_);
        main_shared.main_min_col.push_back(min_);
    }
}


int main(int argc, char *argv[])
{
    string dataset_path = argv[1];
    int path_size = dataset_path.length();
    if (dataset_path[path_size - 1] != '/')
        dataset_path += "/";

    pthread_t thread[NUM_THREADS];
    FILE_PATH path[NUM_THREADS];
    int return_code;
    void* num;
    pthread_mutex_init(&mutex_extremum, NULL);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        path[i].data_path = dataset_path + "train_" + to_string(i) + ".csv";
        return_code = pthread_create(&thread[i], NULL, find_extremums, &path[i]);
        if (return_code)
        {
            cout << "ERROR: Couldn't create thread" << endl;
            exit(-1);
        }
    }

    double data_size = 0;
    for (int i = 0; i < NUM_THREADS; i++)
    {   
        return_code = pthread_join(thread[i], &num);
        if (return_code)
        {
            cout << "ERROR: Couldn't join thread" << endl;
            exit(-1);
        }

        data_size += (double)(long)num;

    }

    set_main_extremums();

    for (int i = 0; i < NUM_THREADS; i++)
    {
        path[i].data_path = dataset_path + "train_" + to_string(i) + ".csv";
        path[i].weights_path = dataset_path + WEIGHTS_FILE_NAME;

        return_code = pthread_create(&thread[i], NULL, predict_data_file, &path[i]);
        if (return_code)
        {
            cout << "ERROR: Could'nt create thread" << endl;
            exit(-1);
        }
    }

    double true_predictions = 0;
    for (int i = 0; i < NUM_THREADS; i++)
    {

        return_code = pthread_join(thread[i], &num);
        if (return_code)
        {
            cout << "ERROR: Couldn't join thread" << endl;
            exit(-1);
        }

        true_predictions += (double)(long)num;

    }

    double accuracy = true_predictions / data_size;
    accuracy *= 100;

    cout << "Accuracy: ";
    cout << fixed;
    cout << setprecision(2);
    cout << accuracy << endl;

    pthread_exit(NULL);
}
