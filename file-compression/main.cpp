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
void bfs(node root);

void create_dictionary(node &node_visited, string code) {
    cout << "4,5\n";
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

    pFile = fopen(file_name, "r");
    string str;
    fread(&str, 512, 1, pFile);
    cout << str;
    node aux, root;
    map<char, int> table;
    priority_queue<node> huff;

    debug << "1\n";

    for (char c : str) {
        table[c]++;
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

    bfs(root);

    create_dictionary(root, "");
    
    for (auto kv : dictionaryAB) {
        cout << kv.first << ' ' << kv.second << endl;
    }
    debug << "5\n";
}

void decompress(const char *file_name) {
    
}

void bfs(node root) {
    queue<node> largura;
    largura.push(root);
    while (!largura.empty()) {
        cout << largura.front().content << ' ' << largura.front().frequency << endl;
        largura.pop();
        for (node child : largura.front().children) {
            largura.push(child);
        }
    }
}