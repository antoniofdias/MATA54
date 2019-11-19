#include <iostream>
#include "huffman.h"

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