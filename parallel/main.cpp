#include "main.hpp"
#include "utils.hpp"
using namespace std;

#define NUM_THREADS 4
#define NUMBER_OF_CLASSES 4
pthread_mutex_t mutex_extremum;
pthread_mutex_t mutex_main_data;

const string WEIGHTS_FILE_NAME = "weights.csv";
string weights_file_path;


// struct {

//     vector<vector<double>> max_col;
//     vector<vector<double>> min_col;

// } shared_extremums;


struct {

    vector<double> main_max_col;
    vector<double> main_min_col;

} main_shared;

struct {
    vector<vector<double>> dataset;
    vector<vector<double>> weigths;
} main_dataset;


typedef struct
{
    string data_path;
    string weights_path;
} FILE_PATH;


int pthread_read_double_csv_file(const string &path)
{
    
    int num = 0;
    const char csv_separator = ',';
    
    fstream file;
    file.open(path);

    string line;
    getline(file, line); // Header Line

    while(getline(file, line))
    {
        num++;
        vector<double> line_words;
        split_str_to_double(line, line_words, csv_separator);
        pthread_mutex_lock(&mutex_main_data);
        main_dataset.dataset.push_back(line_words);
        pthread_mutex_unlock(&mutex_main_data);

    }

    file.close();

    return num;

}



void normilize(vector<vector<double>> &data, int start, int end)
{   
    int data_size = data.size();
    int num_cols = data[0].size() - 1;

    for (int j = 0; j < num_cols; j++)
    {   

        double extremum_diff = main_shared.main_max_col[j] - main_shared.main_min_col[j];
        for (int i = start; i < end; i++)
        {
            pthread_mutex_lock(&mutex_main_data);
            data[i][j] = (data[i][j] - main_shared.main_min_col[j]) / extremum_diff;
            pthread_mutex_unlock(&mutex_main_data);
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


void* extract_data(void* args)
{
    FILE_PATH *file_path_struct = (FILE_PATH*) args;
    string path = file_path_struct->data_path;

    long size = pthread_read_double_csv_file(path);

    pthread_exit((void*) size);
}


void* predict_price_range(void* args)
{
    long tid = (long)args;

    int data_size = main_dataset.dataset.size();
    int goal_column = main_dataset.dataset[0].size() - 1;

    int data_size_for_me = data_size / NUM_THREADS;
    int start = tid * data_size_for_me;
    int end = start + data_size_for_me;
    normilize(main_dataset.dataset, start, end);

    long true_predictions = 0;
    for (int i = start; i < end; i++)
    {
        int predicted = predict_price_class(main_dataset.dataset[i], main_dataset.weigths);
        if (predicted == main_dataset.dataset[i][goal_column])
            true_predictions++;
    }

    pthread_exit((void*)true_predictions);
}


void find_extremum_in_columns()
{
    int data_size = main_dataset.dataset.size();
    int num_cols = main_dataset.dataset[0].size();

    main_shared.main_min_col.clear();
    main_shared.main_max_col.clear();

    for (int j = 0; j < num_cols; j++)
    {
        double max_ = main_dataset.dataset[0][j];
        double min_ = main_dataset.dataset[0][j];

        for (int i = 0; i < data_size; i++)
        {

            if (max_ < main_dataset.dataset[i][j])
                max_ = main_dataset.dataset[i][j];
            if (min_ > main_dataset.dataset[i][j])
                min_ = main_dataset.dataset[i][j];

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
    long tid;
    int return_code;
    void* num;
    pthread_mutex_init(&mutex_extremum, NULL);
    pthread_mutex_init(&mutex_main_data, NULL);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        path[i].data_path = dataset_path + "train_" + to_string(i) + ".csv";
        return_code = pthread_create(&thread[i], NULL, extract_data, &path[i]);
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

    find_extremum_in_columns();
    weights_file_path = dataset_path + WEIGHTS_FILE_NAME;
    read_double_csv_file(weights_file_path, main_dataset.weigths);

    for (int i = 0; i < NUM_THREADS; i++)
    {
        tid = i;
        return_code = pthread_create(&thread[i], NULL, predict_price_range, (void*)tid);
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
    cout << accuracy << '%' << endl;

    pthread_mutex_destroy(&mutex_extremum);
    pthread_mutex_destroy(&mutex_main_data);

    pthread_exit(NULL);
}
