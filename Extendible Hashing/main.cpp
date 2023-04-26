#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "utils.h"
#include <chrono>

#include <sys/resource.h>
#include "hash.h"


using namespace std;

int main()
{
    vector<int> key;
    vector<int> value;
    vector<int> query_keys;
    vector<int> query_remove_keys; 
    int num_rows = 0;
    int num_key_query = 0;
    int num_range_query = 0;
    read_input_file(num_rows, key, value);
    read_key_query_file(num_key_query, query_keys);
    read_remove_query_file(num_range_query, query_remove_keys);


    chrono::steady_clock::time_point start = chrono::steady_clock::now();
    
    //Build index when index constructor is called
    hash_table my_hash_table(1<<1, 2, num_rows, key, value);
    chrono::steady_clock::time_point built_index = chrono::steady_clock::now();
    
    //Query by key
    my_hash_table.key_query(query_keys, "key_query_out1.txt");
    chrono::steady_clock::time_point key_query1 = chrono::steady_clock::now();

    //Remove by key
    my_hash_table.remove_query(query_remove_keys);
    chrono::steady_clock::time_point remove_query = chrono::steady_clock::now();

    //Query by key
    my_hash_table.key_query(query_keys, "key_query_out2.txt");
    chrono::steady_clock::time_point key_query2 = chrono::steady_clock::now();
    
    //Free memory
    my_hash_table.clear();
    
    auto time_to_build_index = chrono::duration_cast<chrono::microseconds>(built_index - start).count();
    auto time_to_query_key1 = chrono::duration_cast<chrono::microseconds>(key_query1 - built_index).count();
    auto time_to_query_remove = chrono::duration_cast<chrono::microseconds>(remove_query - key_query1).count();
    auto time_to_query_key2 = chrono::duration_cast<chrono::microseconds>(key_query2 - remove_query).count();
    record_time_used(int(time_to_build_index), int(time_to_query_key1), int(time_to_query_remove), int(time_to_query_key2));
    return 0;
}
