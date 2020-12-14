#include "utils.hpp"
using namespace std;


void split_str(const string &s, vector<string> &v, const char sep)
{

    v.clear();

    string word;
    istringstream ss(s);
    while(getline(ss, word, sep))
        v.push_back(word);

}


void split_str_to_double(const string &s, vector<double> &v, const char sep)
{

    v.clear();

    string word;
    istringstream ss(s);
    while(getline(ss, word, sep))
        v.push_back(stod(word));

}


void read_double_csv_file(const string &path, vector<vector<double>> &rows)
{

    rows.clear();
    
    const char csv_separator = ',';
    
    fstream file;
    file.open(path);

    string line;
    getline(file, line); // Header Line

    while(getline(file, line))
    {

        vector<double> line_words;
        split_str_to_double(line, line_words, csv_separator);
        rows.push_back(line_words);

    }

    file.close();

}


double inner_multiply(const vector<double> &a, const vector<double> &b, int lim = -1)
{   
    int limit;
    int a_size = a.size();
    int b_size = b.size();
    
    if (lim == -1)
        limit = a_size;
    else if (lim <= a_size)
        limit = lim;
    else
        limit = a_size;
    
    

    double res = 0;
    for (int i = 0; (i < limit) && (i < a_size) && (i < b_size); i++)
        res += a[i] * b[i];
    
    return res;
    
}


void print_2d_vector(const vector<vector<double>> &v)
{
    int length = v.size();
    int width = v[0].size();

    for (int i = 0; i < length; i++)
    {

        for (int j = 0; j < width; j++)
        {
            cout << v[i][j] << "\t";
        }
        cout << endl;

    }
}
