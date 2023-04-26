#ifndef UTILS_H_
#define UTILS_H_

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

void read_input_file(int& num_rows, vector<int>& key, vector<int>& value);
void read_key_query_file(int& num_ke_query, vector<int>& query_kes);
void read_remove_query_file(int& num_remove_query, vector<int>& query_remove_keys);
void record_time_used(int time_to_build_index, int time_to_query_key1, int time_to_query_remove, int time_to_query_key2);
void get_memory_usage();
#endif