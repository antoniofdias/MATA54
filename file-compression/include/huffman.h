#pragma once

#include <bitset>
#include <iostream>
#include <map>
#include <queue>
#include <string.h>

using namespace std;

#define debug if (false) cout
#define pb push_back

struct node {
    bool terminal;
    unsigned int frequency;
    char content;
    vector<node> children;

    bool operator <(const node & cmp_node) const {
        return frequency > cmp_node.frequency;
    }

};

void compress(const char *file_name);
void decompress(const char *file_name);
void create_dictionary(node &node_visited, string code);
map<char, int> read_file_decompressed(FILE* inputFile);
void save_file_compressed(FILE* inputFile, FILE* outputFile);
void read_file_compressed(FILE* inputFile);
void save_file_decompressed(FILE* inputFile, FILE* outputFile);
void compress(const char *file_name);
void decompress(const char *file_name);