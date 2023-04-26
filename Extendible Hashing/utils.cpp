#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "utils.h"

using namespace std;

void read_input_file(int& num_rows, vector<int>& key, vector<int>& value)
{
    //readfile
    fstream file;
    file.open("data.txt");
    string line;

    while(getline(file, line, '\n'))
    {
        istringstream templine(line);
        string data;
        getline(templine, data,',');
        key.push_back(atoll(data.c_str()));
        getline(templine, data,',');
        value.push_back(atoi(data.c_str()));
        num_rows += 1;
    }
    file.close();
    cout << "Data file reading complete, " << num_rows << " rows loaded."<< endl;
    return;
}

void read_key_query_file(int& num_key_query, vector<int>& query_keys)
{
    //readfile
    fstream file;
    file.open("key_query.txt");
    string line;

    while(getline(file, line, '\n'))
    {
        istringstream templine(line);
        string data;
        getline(templine, data, ',');
        query_keys.push_back(atoll(data.c_str()));
        num_key_query += 1;
    }
    file.close();
    cout << "Key query file reading complete, " << num_key_query << " queries loaded."<< endl;
    return;
}

void read_remove_query_file(int& num_remove_query, vector<int>& query_remove_keys){
    //readfile
    fstream file;
    file.open("remove_key_query.txt");
    string line;

    while(getline(file, line, '\n'))
    {
        istringstream templine(line);
        string data;
        getline(templine, data,',');
        query_remove_keys.push_back(atoi(data.c_str()));
        num_remove_query += 1;
    }
    file.close();
    cout << "Remove query file reading complete, " << num_remove_query << " queries loaded."<< endl;
    return;
}


void record_time_used(int time_to_build_index, int time_to_query_key1, int time_to_query_remove, int time_to_query_key2)
{
    ofstream file ("time_used.txt");
    if(file.is_open())
    {
        file << time_to_build_index << "," << time_to_query_key1 << "," << time_to_query_remove << "," << time_to_query_key2 << endl;
        file.close();
    }
}


// void get_memory_usage(){
//     std::ifstream statFile("/proc/self/stat");
//     std::string statLine;
//     std::getline(statFile, statLine);
//     std::istringstream iss(statLine);
//     std::string entry;
//     long long memUsage;
//     for (int i = 1; i <= 24; i++) {
//         std::getline(iss, entry, ' ');
//         if (i == 24) {
//             memUsage = stoi(entry);
//         }
//     }

//     std::cout << "ram = " << memUsage << " (4KB)" << std::endl;
    
// }