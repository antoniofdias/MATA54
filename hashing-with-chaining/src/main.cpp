#include <iostream>
#include <cstdio> 
#include <unistd.h>
#include <tuple>
#include <iomanip>

#include "file.hpp"

using namespace std;

int main() {

    int key;
    char op;
    string content;
    FILE * pFile = start_file("./file.bin");
    tuple<int, int, int> footer = read_footer(pFile);

    while (cin >> op, op != 'e') {

        switch (op) {
            case 'i':
                cin >> key;
                cin.ignore();
                getline(cin, content);
                insert_record(key, content, pFile, footer);
                break;
            case 'c':
                cin >> key;
                find_record(key, pFile);
                break;
            case 'r':
                cin >> key;
                remove_record(key, pFile, footer);
                break;
            case 'p':
                print_file(get<0>(footer), pFile);
                break;
            case 'm':
                access_average(footer);
                break;
            default:
                break;
        }

    }

    update_footer(pFile, footer);
    fclose(pFile);

}