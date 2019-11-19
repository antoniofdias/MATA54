#include <bitset>
#include <iostream>
#include <map>
#include <queue>
#include <string.h>
#include "huffman.h"

using namespace std;

#define debug if (false) cout
#define pb push_back

map<char, string> dictionaryAB;
map<string, char> dictionaryBA;

void create_dictionary(node &node_visited, string code) {
    if (!node_visited.terminal) {
        create_dictionary(node_visited.children[1], code + '0');
        create_dictionary(node_visited.children[0], code + '1');
    }
    else {
        dictionaryAB[node_visited.content] = code;
    }
}

map<char, int> read_file_decompressed(FILE* inputFile) {
    map<char, int> table;

    fseek(inputFile, 0, SEEK_END);
    int file_size = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);
    debug << file_size << endl;

    for (int i = 0; i < file_size; i++) {
        char buffer;
        fread(&buffer, sizeof(char), 1, inputFile);
        table[buffer]++;
        debug << buffer;
    } 

    debug << endl;
    return table;
}

void save_file_compressed(FILE* inputFile, FILE* outputFile) {
    debug << "HEADER" << endl;
  
    int size = dictionaryAB.size();
    fwrite(&size, sizeof(int), 1, outputFile);
    for (auto item : dictionaryAB) {
        fwrite(&item.first, sizeof(char), 1, outputFile);
        int len = item.second.size();
        fwrite(&len, sizeof(int), 1, outputFile);
        for (char c : item.second)
            fwrite(&c, sizeof(char), 1, outputFile);
        debug << item.first << "[" << len << "]: " << item.second << endl;
    }

    debug << "CONTENT" << endl;

    fseek(inputFile, 0, SEEK_END);
    int file_size = ftell(inputFile);
    fseek(inputFile, 0, SEEK_SET);

    bitset<8> bs = 0;
    int rep_size = 0;
    for (int i = 0; i < file_size; i++) {
        char buffer;
        fread(&buffer, sizeof(char), 1, inputFile);
        debug << dictionaryAB[buffer] << "(" << buffer  << ")";
        int len = dictionaryAB[buffer].size();

        for (int i=0; i < len; i++) {
            bs = bs << 1;
            rep_size++;
            
            if (dictionaryAB[buffer][i] == '1'){
                bs |= 1;
            }
            if (rep_size % 8 == 0) {
                char representacao = (char)bs.to_ulong();
                fwrite(&representacao, sizeof(char), 1, outputFile);
                rep_size = 0;
            }
        }
    } 
    if (rep_size) {
        bs = bs << (8-rep_size);
        char representacao = (char)bs.to_ulong();
        
        fwrite(&representacao, sizeof(char), 1, outputFile);
    }
    debug << endl;

}

void read_file_compressed(FILE* inputFile) {
    debug << "HEADER" << endl;

    int size;
    fread(&size, sizeof(int), 1, inputFile);
    for (int i = 0; i < size; i++) {
        char character;
        fread(&character, sizeof(char), 1, inputFile);

        int len;
        fread(&len, sizeof(int), 1, inputFile);

        string representacao = "";
        for (int i = 0; i < len; i++) {
            char buffer;
            fread(&buffer, sizeof(char), 1, inputFile);
            representacao += buffer;
        }
        debug << character << "[" << len << "]: " << representacao << endl;

        dictionaryBA[representacao] = character;
    }
}

void save_file_decompressed(FILE* inputFile, FILE* outputFile) {
    int pos = ftell(inputFile);
    fseek(inputFile, 0, SEEK_END);
    int len = ftell(inputFile) - pos;
    fseek(inputFile, pos, SEEK_SET);

    debug << "CONTENT" << endl;
    char buffer;
    string representacao;

    for (int i = 0; i < len; i++) {
        fread(&buffer, sizeof(char), 1, inputFile);
        bitset<8> bs(buffer);
        for (int j = 7; j >= 0; j--) {
            if (bs[j] == 1)
                representacao += '1';
            else
                representacao += '0';

            if (dictionaryBA.count(representacao)) {
                fwrite(&dictionaryBA[representacao], sizeof(char), 1, outputFile);
                debug << representacao << "(" << dictionaryBA[representacao] << ")";
                representacao = "";
            }
        }
    }
}

void compress(const char *file_name) {
    FILE *inputFile = fopen(file_name, "r");
    string file_name_str = file_name;
    file_name_str += ".cmp";
    const char* another_file_name = file_name_str.c_str();
    FILE *outputFile = fopen(another_file_name, "w");

    debug << file_name << endl;

    node aux, root;
    priority_queue<node> huff;

    map<char, int> table = read_file_decompressed(inputFile);

    for (auto e : table) {
        aux.terminal = true;
        aux.frequency = e.second;
        aux.content = e.first;
        huff.push(aux);
    }

    while (!huff.empty()) {
        node left = huff.top();
        huff.pop();
        if (huff.empty()) {
            root = left;
        }
        else {
            node right = huff.top(), next;
            huff.pop();

            next.terminal = false;
            next.frequency = right.frequency + left.frequency;
            next.content = 64;
            next.children.pb(right);
            next.children.pb(left);
            huff.push(next);
        }
    }
    create_dictionary(root, "");

    save_file_compressed(inputFile, outputFile);

    fclose(inputFile);
    fclose(outputFile);
}

void decompress(const char *file_name) {
    FILE *inputFile = fopen(file_name, "r");
    string aux(file_name + 0, ((string)file_name).size() - 4);
    const char* another_file_name = aux.c_str();
    FILE *outputFile = fopen(another_file_name, "w");

    read_file_compressed(inputFile);

    save_file_decompressed(inputFile, outputFile);

    fclose(inputFile);
    fclose(outputFile);
}