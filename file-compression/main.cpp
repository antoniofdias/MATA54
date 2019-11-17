#include <bitset>
#include <iostream>
#include <map>
#include <queue>
#include <string.h>

using namespace std;

#define debug if (true) cout
#define pb push_back

map<char, string> dictionaryAB;
map<string, char> dictionaryBA;



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

void create_dictionary(node &node_visited, string code) {
    if (!node_visited.terminal) {
        create_dictionary(node_visited.children[1], code + '0');
        create_dictionary(node_visited.children[0], code + '1');
    }
    else {
        dictionaryAB[node_visited.content] = code;
    }
}

int main (int argc, const char *argv[]) {
    if (argc == 3) {
        switch(argv[1][1]) {
            case 'c':
                compress(argv[2]);
                break;
            case 'd':
                decompress(argv[2]);
                break;
            default:
                cerr << "Please make sure you want to compress (-c) or decompress (-d) a file.\n";
                return 1;
        }
    }
    else {
        cerr << "Please check the number of parameters\n";
        return 1;
    }
}

map<char, int> read_file(FILE* inputFile) {
    map<char, int> table;
    char buffer;
    while ((buffer = getc(inputFile)) != EOF) {
        table[buffer]++;
    } 
    fclose(inputFile);
    return table;
}

void save_file_compressed(FILE* inputFile, FILE* outputFile) {
  
    int size = dictionaryAB.size();
    fwrite(&size, sizeof(int), 1, outputFile);
    for (auto item : dictionaryAB) {
        fwrite(&item.first, sizeof(char), 1, outputFile);
        int len = item.second.size();
        fwrite(&len, sizeof(int), 1, outputFile);
        bitset<1> um = 1;
        bitset<1> zero = 0;
        for (int i = 0; i < len; i++) {
            if (item.second[i] == 0)
                fwrite(&zero, sizeof(zero), 1, outputFile);
            else
                fwrite(&um, sizeof(um), 1, outputFile);
        }
    }
    char buffer;
    while ((buffer = getc(inputFile)) != EOF) {
        int len = dictionaryAB[buffer].size();
        fwrite(&dictionaryAB[buffer], len * sizeof(char), 1, outputFile);
    } 


}

void read_file_compressed_header(FILE* inputFile) {
    int size;
    fread(&size, sizeof(int), 1, inputFile);
    for (int i = 0; i < size; i++) {
        char character;
        fread(&character, sizeof(char), 1, inputFile);

        int len;
        fread(&len, sizeof(int), 1, inputFile);

        string representacao = "";
        for (int i = 0; i < len; i++) {
            bitset<1> buffer;
            fread(&buffer, sizeof(buffer), 1, inputFile);
            if (buffer == 1)
                representacao += '1';
            else
                representacao += '0';
        }
        dictionaryBA[representacao] = character;
    }
}

void compress(const char *file_name) {
    FILE *inputFile = fopen(file_name, "r");
    string file_name_str = file_name;
    file_name_str += ".cmp";
    const char* another_file_name = file_name_str.c_str();
    FILE *outputFile = fopen(another_file_name, "w");

    cout << file_name << endl;

    node aux, root;
    priority_queue<node> huff;

    map<char, int> table = read_file(inputFile);

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

    read_file_compressed_header(inputFile);

    fclose(inputFile);
    fclose(outputFile);
}