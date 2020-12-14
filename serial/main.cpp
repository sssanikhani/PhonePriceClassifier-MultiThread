#include "main.hpp"
#include "utils.hpp"
using namespace std;

#define NUMBER_OF_CLASSES 4
#define NUM_PRECISION 2

const string DATASET_FILE_NAME = "train.csv";
const string WEIGHTS_FILE_NAME = "weights.csv";


void find_extremum_in_columns(
    const vector<vector<double>> &data, 
    vector<double> &max_col, 
    vector<double> &min_col
)
{
    int data_size = data.size();
    int num_cols = data[0].size();

    min_col.clear();
    max_col.clear();

    for (int j = 0; j < num_cols; j++)
    {
        double max_ = data[0][j];
        double min_ = data[0][j];

        for (int i = 0; i < data_size; i++)
        {

            if (max_ < data[i][j])
                max_ = data[i][j];
            if (min_ > data[i][j])
                min_ = data[i][j];

        }
        max_col.push_back(max_);
        min_col.push_back(min_);
    }

}


void normilize(vector<vector<double>> &data)
{   
    int data_size = data.size();
    int num_cols = data[0].size() - 1;

    vector<double> max_col, min_col;
    find_extremum_in_columns(data, max_col, min_col);

    for (int j = 0; j < num_cols; j++)
    {
        double extremum_diff = max_col[j] - min_col[j];
        for (int i = 0; i < data_size; i++)
        {

            data[i][j] = (data[i][j] - min_col[j]) / extremum_diff;

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


int main(int argc, char *argv[])
{
    string dataset_dir = argv[1];
    
    vector<string> headers;
    vector<vector<double>> dataset;
    vector<vector<double>> weights;

    int dataset_dir_length = dataset_dir.length();
    if (dataset_dir[dataset_dir_length - 1] != '/')
        dataset_dir += '/';
    string dataset_path = dataset_dir + DATASET_FILE_NAME;
    string weights_path = dataset_dir + WEIGHTS_FILE_NAME;

    read_double_csv_file(dataset_path, headers, dataset);
    read_double_csv_file(weights_path, headers, weights);
    normilize(dataset);

    int data_size = dataset.size();
    int goal_column = dataset[0].size() - 1;
    vector<int> predicts;


    for (int i = 0; i < data_size; i++)
    {   
        int predicted_price_range = predict_price_class(dataset[i], weights);
        predicts.push_back(predicted_price_range);
    }

    double true_predicts = 0;
    for (int i = 0; i < data_size; i++)
    {   
        if (predicts[i] == dataset[i][goal_column])
            true_predicts++;
    }
    double accuracy = true_predicts / data_size;
    accuracy *= 100;

    cout << "Accuracy: ";
    cout << fixed;
    cout << setprecision(NUM_PRECISION);
    cout << accuracy << '%' << endl;

    return 0;

}