#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include "hash.h"
#include <bitset>
#include "utils.h"

using namespace std;

hash_entry::hash_entry(int key, int value){
    this->key = key;
    this->value = value;
    this->next = nullptr;

}

hash_bucket::hash_bucket(int hash_key, int depth){
    this->local_depth = depth;
    this->num_entries = 0;
    this->hash_key= hash_key;
    this->first = nullptr;



}

/* Free the memory alocated to this->first
*/
void hash_bucket::clear(){
     hash_entry *current = this->first;
    while (current != nullptr) {
        hash_entry *temp = current;
        current= current->next;
        delete temp;
    }
    this->first = nullptr;
    this->num_entries = 0;
    
}

hash_table::hash_table(int table_size, int bucket_size, int num_rows, vector<int> key, vector<int> value){
    // initialize hash table
    this->table_size = table_size;
    this->bucket_size = bucket_size;
    this->global_depth = 1;
    this->bucket_table = vector<hash_bucket*>(table_size);
    for (int i = 0; i < table_size; i++) {
        this->bucket_table[i] = new hash_bucket(i, this->global_depth);
    }
    // insert rows
    for (int i = 0; i < num_rows; i++) {
        insert(key[i], value[i]);
    }

    
}

/* When insert collide happened, it needs to do rehash and distribute the entries in the bucket.
** Furthermore, if the global depth equals to the local depth, you need to extend the table size.
*/
void hash_table::extend(hash_bucket *bucket) {
    // Split the bucket into two new buckets
    hash_bucket *bucket1 = new hash_bucket(bucket->hash_key, bucket->local_depth + 1);
    hash_bucket *bucket2 = new hash_bucket(bucket->hash_key + (int)pow(2, bucket1->local_depth - 1), bucket1->local_depth);

    // Iterate over all items in the bucket and rehash them into the new buckets
        hash_entry *entry = bucket->first;
        hash_entry *prev = nullptr;
        while (entry) {
            int new_hash_key = entry->key % (int)pow(2, bucket1->local_depth) /(int) pow(2, bucket1->local_depth - 1);
        if (new_hash_key == 0) {
            hash_entry *entry1 = bucket1->first;
            hash_entry *prev1 = nullptr;
            while (entry1){
                prev1=entry1;
                entry1 = entry1->next;
            }
            hash_entry *new_entry = new hash_entry(entry->key, entry->value);
            if(prev1==nullptr){
                bucket1->first = new_entry;
            } 
            else{
                prev1->next = new_entry;
            } 
            bucket1->num_entries ++;
            }
            else {
            hash_entry *entry2 = bucket2->first;
            hash_entry *prev2 = nullptr;
            while (entry2){
                prev2=entry2;
                entry2 = entry2->next;
            }
            hash_entry *new_entry = new hash_entry(entry->key, entry->value);
            if(prev2==nullptr){
                bucket2->first = new_entry;
            } 
            else{
                prev2->next = new_entry;
            } 
            bucket2->num_entries ++;
            }
            
            prev = entry;
            entry = entry->next;
            delete prev;
        }
        
        if (bucket->local_depth == global_depth) {
            // Double the size of the bucket table and repeat the items
            table_size = bucket_table.size();
            for (int i = 0; i < table_size; i++) {
                bucket_table.push_back(bucket_table[i]);
            }
            table_size = table_size * 2;
            global_depth = global_depth + 1;
        }

        // Update the bucket table with the new buckets
       for (int i=bucket1->hash_key; i<table_size; i+= (int)pow(2, bucket1->local_depth)) bucket_table[i]= bucket1;
       for (int i=bucket2->hash_key; i<table_size; i+= (int)pow(2, bucket2->local_depth)) bucket_table[i]= bucket2;
        // Delete the original bucket
        delete bucket;
    }


/* When construct hash_table you can call insert() in the for loop for each key-value pair.
*/
void hash_table::insert(int key, int value){
    int index = key % table_size;
    hash_bucket *bucket = bucket_table[index];
    int bucket_depth = bucket->local_depth;

    hash_entry *entry = bucket->first;
    hash_entry *prev = nullptr;
    while (entry) {
        if (entry->key == key) {
            entry->value = value;
            return;
        }
        prev = entry;
        entry = entry->next;
    }

    if (bucket->num_entries < bucket_size) {
        hash_entry *new_entry = new hash_entry(key, value);
        if (prev == nullptr) {
            bucket->first = new_entry;
        } else {
            prev->next = new_entry;
        }
        bucket->num_entries++;
    } else {
            extend(bucket);
            insert( key,  value);
    }
    
}

/* The function might be called when shrink happened.
** Check whether the table necessory need the current size of table, or half the size of table
*/
void hash_table::half_table() {
    for (int i=0 ; i< table_size ; i++){
        if (bucket_table[i]-> local_depth>= global_depth){
            return;
        }
    }
    if(global_depth > 1){
        global_depth --;
        table_size /=2;
        for(int i =0; i< table_size ; i++){
            bucket_table.pop_back();
        }
    }
}



/* If a bucket with no entries, it need to check whether the pair hash index bucket 
** is in the same local depth. If true, then merge the two bucket and reassign all the 
** related hash index. Or, keep the bucket in the same local depth and wait until the bucket 
** with pair hash index comes to the same local depth.
*/
void hash_table::shrink(hash_bucket *bucket) {
  
    if (bucket->num_entries > 0) return ;
    int pair_index;
    if(bucket->hash_key/ (int) pow (2, bucket->local_depth-1)){
        pair_index = bucket->hash_key - (int)pow(2, bucket->local_depth-1);
            
    }else{
            pair_index = bucket-> hash_key + (int)pow(2, bucket->local_depth-1);
    }
        hash_bucket *pair = bucket_table[pair_index];
        if(bucket-> local_depth > 1 && bucket->local_depth==pair-> local_depth){
        for (int i= bucket->hash_key ; i < table_size ; i+= (int)pow(2, bucket->local_depth))
             bucket_table[i]=pair;
        pair->local_depth--;
        pair->hash_key = pair->hash_key % (int) pow(2, pair->local_depth);
        delete bucket;
        shrink(pair);
        half_table();
        }

}
/* When executing remove_query you can call remove() in the for loop for each key.
*/
void hash_table::remove(int key) {
    // Calculate hash index of key
    int hash_index = key % table_size;

    // Find the bucket
    hash_bucket* bucket = bucket_table[hash_index];

    // Search for the entry with matching key
    hash_entry* entry = bucket->first;
    hash_entry* prev = nullptr;
    while (entry != nullptr && entry->key != key) {
        prev = entry;
        entry = entry->next;
    }

    // If found, remove the entry
    if (entry != nullptr) {
        if (prev == nullptr) {
            // The first entry in the bucket matches
            bucket->first = entry->next;
        } else {
            // An entry further down the list matches
            prev->next = entry->next;
        }

        // Delete the removed entry
        delete entry;

        // Update the number of entries in the bucket
        bucket->num_entries--;
        shrink(bucket);
    }
}


void hash_table::key_query(vector<int> query_keys, string file_name){
    //cout << "queries" << endl;
    ofstream outfile(file_name); // open output file
    for (int i = 0; i < query_keys.size(); i++) {
       int key = query_keys[i];
       int index = key % table_size;
       hash_bucket *bucket = bucket_table[index];

       bool found = false;
       hash_entry *entry = bucket->first;
       
    while (entry) {
        if (entry->key == key) {
            found=true;
            outfile<<entry->value<< "," << bucket->local_depth << endl;
            break;
        }
        
        entry = entry->next;
    }
    if(!found){
        outfile<<"-1"<<","<< bucket->local_depth<<endl;
    }


    
}
outfile.close(); // close output file

    
}

void hash_table::remove_query(vector<int> query_keys){
    for (int i = 0; i < query_keys.size(); i++) {
       int key = query_keys[i];
       remove(query_keys[i]);
    }
    for (int i = 0; i<table_size; i++) shrink(bucket_table[i]);
}

/* Free the memory that you have allocated in this program
*/
void hash_table::clear() {
    for (int i = 0; i < bucket_table.size(); i++) {
        hash_bucket *tmp =bucket_table[i];
       tmp ->clear();
       bucket_table.erase(std::remove(bucket_table.begin(), bucket_table.end(), tmp), bucket_table.end());
        delete tmp;
    }
    
}
