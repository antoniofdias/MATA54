#include <iostream>
#include <queue>
#include <map>

using namespace std;

#define debug if (true) cout

struct node {
    bool terminal;
    unsigned int frequency;
    char content;
    node *left_child = NULL;
    node *right_child = NULL;

    bool operator <(const node & cmp_node) const {
        return frequency > cmp_node.frequency;
    }

};

int main () {
    string str;
    node aux, root;
    map<char, int> table;
    priority_queue<node> huff;
    queue<node> bfs;

    cin >> str;

    for (char c : str) {
        table[c]++;
    }

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
            node right = huff.top();
            huff.pop();
            aux.terminal = false;
            aux.frequency = right.frequency + left.frequency;
            aux.content = 64;
            aux.right_child = &right;
            aux.left_child = &left;
            huff.push(aux);
        }
    }

    bfs.push(root);
    while (!bfs.empty()) {
        aux = bfs.front();
        bfs.pop();
        cout << "Conteúdo: " << aux.content << ", Frequência: " << aux.frequency << endl;
        //if (aux.left_child != NULL) bfs.push(*aux.left_child);
        if (aux.right_child != NULL) bfs.push(*aux.right_child);
    }
}