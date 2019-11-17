#include <iostream>
#include <queue>
#include <map>

using namespace std;

#define debug if (true) cout
#define pb push_back

map<char, string> dictionaryAB;
map<char, string> dictionaryBA;

FILE *pFile;

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

void compress(const char *file_name) {
    cout << file_name << endl;

    pFile = fopen(file_name, "r");
    char str[512];
    int str_size = fread(&str, sizeof(char), 512, pFile);
    node aux, root;
    map<char, int> table;
    priority_queue<node> huff;

    debug << str;

    for (int i = 0; i < str_size - 1; i++) {
        table[str[i]]++;
    }
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
    
    for (auto kv : dictionaryAB) {
        cout << kv.first << ' ' << kv.second << endl;
    }
    debug << "5\n";
}

void decompress(const char *file_name) {
    
}