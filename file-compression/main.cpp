#include <iostream>
#include <queue>
#include <string.h>
#include <map>

using namespace std;

#define debug if (true) cout
#define pb push_back

map<char, string> dictionaryAB;
map<char, string> dictionaryBA;



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

map<char, int> read_file(const char *file_name) {
    FILE *inputFile = fopen(file_name, "r");

    map<char, int> table;
    char buffer;
    while ((buffer = getc(inputFile)) != EOF) {
        table[buffer]++;
        debug << buffer;
    } 
    debug << endl;
    fclose(inputFile);
    return table;
}

void save_file_compressed(const char *file_name) {
    FILE *inputFile = fopen(file_name, "r");
    string aux = file_name;
    aux += ".cmp";
    const char* another_file_name = aux.c_str();
    FILE *outputFile = fopen(another_file_name, "w");

    for (auto item : dictionaryAB) {
        fwrite(&item.first, sizeof(char), 1, outputFile);
        int len = item.second.size();
        fwrite(&len, sizeof(int), 1, outputFile);
        fwrite(&item.second, len * sizeof(char), 1, outputFile);
    }
    char buffer;
    while ((buffer = getc(inputFile)) != EOF) {
        int len = dictionaryAB[buffer].size();
        fwrite(&dictionaryAB[buffer], len * sizeof(char), 1, outputFile);
    } 

    fclose(inputFile);
    fclose(outputFile);
}

void compress(const char *file_name) {
    cout << file_name << endl;

    node aux, root;
    priority_queue<node> huff;

    debug << "1\n";
    map<char, int> table = read_file(file_name);
    debug << "2\n";

    for (auto e : table) {
        aux.terminal = true;
        aux.frequency = e.second;
        aux.content = e.first;
        huff.push(aux);
    }
    debug << "3\n";

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
    debug << "4\n";

    create_dictionary(root, "");

    debug << "Q\n";

    save_file_compressed(file_name);

    debug << "5\n";
}

void decompress(const char *file_name) {
    
}