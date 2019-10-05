#pragma once

#include <iostream>
#include <cstdio> 
#include <unistd.h>
#include <tuple>
#include <iomanip>

using namespace std;

struct record {
    unsigned int key;
    char content[21];
    bool empty;
    int prev;
    int next;
    int chain_size;
};

const int FILE_SIZE = 11;

record read(int position, FILE * pFile);
void write(int position, record to_write, FILE * pFile);
FILE * start_file(const char* file_name);
tuple<int, int, int> read_footer(FILE * pFile);
void update_footer(FILE * pFile, tuple<int, int, int> &footer);
void insert_record(unsigned int key, string content, FILE * pFile, tuple<int, int, int> &footer);
void find_record(unsigned int key, FILE * pFile);
void remove_record(unsigned int key, FILE * pFile, tuple<int, int, int> &footer);
void print_file(int empty_list_start, FILE * pFile);
void access_average(tuple<int, int, int> &footer);
void update_empty_list(int occupied_position, char operation, FILE * pFile, tuple<int, int, int> &footer);