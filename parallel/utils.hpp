#ifndef UTILS_H
#define UTILS_H

#include "main.hpp"
#include <sstream>
#include <fstream>


void split_str(const std::string &s, std::vector<std::string> &v, const char sep);

void split_str_to_double(const std::string &s, std::vector<double> &v, const char sep);

void read_double_csv_file(
    const std::string &path, 
    std::vector<std::string> &headers, 
    std::vector<std::vector<double>> &rows
);

double inner_multiply(const std::vector<double> &a, const std::vector<double> &b, int lim);

void print_2d_vector(const std::vector<std::vector<double>> &v);


void find_extremum_in_columns(
    const std::vector<std::vector<double>> &data, 
    std::vector<double> &max_col, 
    std::vector<double> &min_col
);

#endif