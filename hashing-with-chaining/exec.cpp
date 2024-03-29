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

record read(int position);
void write (int position, record to_write);
FILE * start_file(const char* file_name);
tuple<int, int, int> read_footer();
void update_footer();
void insert_record(unsigned int key, string content);
void find_record(unsigned int key);
void remove_record(unsigned int key);
void print_file(int empty_list_start);
void access_average();
void update_empty_list(int occupied_position, char operation);

int FILE_SIZE = 11;
FILE * pFile;
tuple<int, int, int> footer;

int main() {

    int key;
    char op;
    string content;
    pFile = start_file("./src/file.bin");
    footer = read_footer();

    while (cin >> op, op != 'e') {

        switch (op) {
            case 'i':
                cin >> key;
                cin.ignore();
                getline(cin, content);
                insert_record(key, content);
                break;
            case 'c':
                cin >> key;
                find_record(key);
                break;
            case 'r':
                cin >> key;
                remove_record(key);
                break;
            case 'p':
                print_file(get<0>(footer));
                break;
            case 'm':
                access_average();
                break;
            default:
                break;
        }

    }

    update_footer();
    fclose(pFile);

}

tuple<int, int, int> read_footer() {

    tuple<int, int, int> footer;

    fseek(pFile, (sizeof(int) * (-3)), SEEK_END);
    fread(&get<0>(footer), sizeof(int), 1, pFile);
    fread(&get<1>(footer), sizeof(int), 1, pFile);
    fread(&get<2>(footer), sizeof(int), 1, pFile);

    return footer;

}

int hash_function(int key) {
    return (key % FILE_SIZE);
}

record read(int position) {

    record buffer;
    fseek(pFile, sizeof(struct record) * position, SEEK_SET);
    fread(&buffer, sizeof(struct record), 1, pFile);
    return buffer;

}

void write (int position, record to_write) {

    fseek(pFile, sizeof(struct record) * position, SEEK_SET);
    fwrite(&to_write, sizeof(struct record), 1, pFile);

}

FILE * start_file(const char* file_name) {

    if (access(file_name, F_OK) == -1) {

        FILE * new_file = fopen(file_name, "wb");
        record new_record;

        for (int i = 0; i < FILE_SIZE; i++) {
            new_record.key = -1, new_record.empty = true;
            new_record.prev = (i + FILE_SIZE - 1) % FILE_SIZE;
            new_record.next = (i + 1) % FILE_SIZE;
            fwrite(&new_record, sizeof(struct record), 1, new_file);
        }

        int empty_list_start = FILE_SIZE - 1, occupied_positions = 0, chain_sizes = 0;
        fwrite(&FILE_SIZE, sizeof(int), 1, new_file);
        fwrite(&empty_list_start, sizeof(int), 1, new_file);
        fwrite(&occupied_positions, sizeof(int), 1, new_file);
        fwrite(&chain_sizes, sizeof(int), 1, new_file);

        fclose(new_file);
    }

    FILE * fTest = fopen(file_name, "rb+");
    int test_file_size;
    fseek(fTest, (sizeof(int) * (-4)), SEEK_END);
    fread(&test_file_size, sizeof(int), 1, fTest);

    if (test_file_size != FILE_SIZE) {
        throw runtime_error("FILE_SIZE mismatch.");
    }
    else {
        return fTest;
    }
}

void update_footer() {

    fseek(pFile, (sizeof(int) * (-3)), SEEK_END);
    fwrite(&get<0>(footer), sizeof(int), 1, pFile);
    fwrite(&get<1>(footer), sizeof(int), 1, pFile);
    fwrite(&get<2>(footer), sizeof(int), 1, pFile);

}

record create_record(int key, string content) {

    record new_record;
    new_record.key = key;
    for (unsigned int i = 0; i < content.size(); i++) {
        new_record.content[i] = content[i];
    }
    new_record.content[content.size()] = '\0';
    new_record.empty = false;
    new_record.next = new_record.prev = -1;
    return new_record;

}


void insert_record(unsigned int key, string content) {

    record new_record, old_record;
    new_record = create_record(key, content);
    old_record = read(hash_function(key));

    if (get<1>(footer) == FILE_SIZE) {
        throw runtime_error("FILE_SIZE limit reached.");
    }
    
    if (old_record.empty) {
        get<1>(footer)++;
        update_empty_list(hash_function(key), 'r');
        new_record.chain_size = 1;
        get<2>(footer) += new_record.chain_size;
        write (hash_function(key), new_record);
    }
    else {

        if (hash_function(old_record.key) == hash_function(key)) {

            int previous_position = hash_function(key);
            while (old_record.next != -1 && old_record.key != key) {
                previous_position = old_record.next;
                old_record = read(old_record.next);
            }

            if (old_record.key != key){
                get<1>(footer)++;
                old_record.next = get<0>(footer);
                update_empty_list(get<0>(footer), 'r');
                write(previous_position, old_record);
                write(old_record.next, new_record);

                record chain_head = read(hash_function(key));
                chain_head.chain_size++;
                write(hash_function(key), chain_head);
                get<2>(footer) += chain_head.chain_size;
            }
            else {
                cout << "chave ja existente: " << key << endl;
            }

        }

        else {

            int previous_position = hash_function(old_record.key);
            record old_parent = read(previous_position);
            while (old_parent.next != hash_function(key)) {
                previous_position = old_parent.next;
                old_parent = read(old_parent.next);
            }

            old_parent.next = get<0>(footer);
            update_empty_list(get<0>(footer), 'r');
            write(previous_position, old_parent);
            write(old_parent.next, old_record);

            get<1>(footer)++;
            new_record.chain_size = 1;
            get<2>(footer) += new_record.chain_size;
            write(hash_function(key), new_record);

        }

    }

}

void update_empty_list(int occupied_position, char operation) {

    record overwritten, adjacent;

    if (operation == 'r') {
        overwritten = read(occupied_position);
        
        adjacent = read(overwritten.next);
        adjacent.prev = overwritten.prev;
        write(overwritten.next, adjacent);
        
        adjacent = read(overwritten.prev);
        adjacent.next = overwritten.next;
        write(overwritten.prev, adjacent);

        if (occupied_position == get<0>(footer)) {
            get<0>(footer) = overwritten.prev;
        }
        
        if (get<1>(footer) == FILE_SIZE) {
            get<0>(footer) = -1;
        }
    }

    else {
        overwritten = create_record(-1, "");
        overwritten.empty = true;

        if (get<0>(footer) != -1) {
            record previous_empty_head = read(get<0>(footer));
            overwritten.next = get<0>(footer);
            overwritten.prev = previous_empty_head.prev;
            previous_empty_head.prev = occupied_position;
            write(get<0>(footer), previous_empty_head);
            get<0>(footer) = occupied_position;

            adjacent = read(overwritten.prev);
            adjacent.next = occupied_position;
            write(overwritten.prev, adjacent);
        }
        else {
            get<0>(footer) = occupied_position;
            overwritten.next = overwritten.prev = occupied_position;
        }

        write(occupied_position, overwritten);
    }

}


void find_record(unsigned int key) {
    record current_record = read(hash_function(key));

    while (current_record.next != -1 && current_record.key != key){
        current_record = read(current_record.next);
    }

    if (current_record.key == key) {
        cout << "chave: " << current_record.key << endl << current_record.content << endl;
    }
    else {
        cout << "chave nao encontrada: " << key << endl;
    }
}

void remove_record(unsigned int key) {

    int previous_position = hash_function(key), father_position = hash_function(key);
    record sought_record = read(previous_position);
    while (sought_record.next != -1 && sought_record.key != key) {
        father_position = previous_position;
        previous_position = sought_record.next;
        sought_record = read(sought_record.next);
    }
    
    if (sought_record.key == key) {

        get<1>(footer)--;
        record chain_head = read(hash_function(key));
        int chain_size = chain_head.chain_size;

        if (sought_record.next != -1) {
            record sought_child = read(sought_record.next);
            write(previous_position, sought_child);
            update_empty_list(sought_record.next, 'i');
        }

        else {
            record sought_father = read(father_position);
            sought_father.next = -1;
            write(father_position, sought_father);
            update_empty_list(previous_position, 'i');
        }

        chain_head = read(hash_function(key));
        get<2>(footer) -= chain_size;
        chain_head.chain_size = chain_size - 1;
        write(hash_function(key), chain_head);

    }
    else {
        cout << "chave nao encontrada: " << key << endl;
    }
}

void print_file(int empty_list_start) {

    record buffer;
    fseek(pFile, 0, SEEK_SET);
    for (int i = 0; i < FILE_SIZE; i++) {
        fread(&buffer, sizeof(struct record), 1, pFile);
        cout << i << ": ";
        if (buffer.empty) {
            cout << "apontador1: " << buffer.prev << " apontador2: " << buffer.next << endl;
        }
        else {
            cout << buffer.key << ' ' << buffer.content;
            cout << ' ' << (buffer.next != -1 ? to_string(buffer.next) : "nulo") << endl;
        }
    }
    cout << "posicao inicial da lista de posicoes vazias: " << (empty_list_start != -1 ? to_string(empty_list_start) : "nulo") << endl;

}

void access_average() {
    double average = (get<1>(footer) ? (double) get<2>(footer) / get<1>(footer) : 0.0);
    cout << setprecision(1) << fixed << average << endl;
}