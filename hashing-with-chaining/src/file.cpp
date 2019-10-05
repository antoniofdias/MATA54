#include "file.hpp"

tuple<int, int, int> read_footer(FILE * pFile) {

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

record read(int position, FILE * pFile) {

    record buffer;
    fseek(pFile, sizeof(struct record) * position, SEEK_SET);
    fread(&buffer, sizeof(struct record), 1, pFile);
    return buffer;

}

void write(int position, record to_write, FILE * pFile) {

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

void update_footer(FILE * pFile, tuple<int, int, int> &footer) {

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


void insert_record(unsigned int key, string content, FILE * pFile, tuple<int, int, int> &footer) {

    record new_record, old_record;
    new_record = create_record(key, content);
    old_record = read(hash_function(key), pFile);

    if (get<1>(footer) == FILE_SIZE) {
        throw runtime_error("FILE_SIZE limit reached.");
    }
    
    if (old_record.empty) {
        get<1>(footer)++;
        update_empty_list(hash_function(key), 'r', pFile, footer);
        new_record.chain_size = 1;
        get<2>(footer) += new_record.chain_size;
        write(hash_function(key), new_record, pFile);
    }
    else {

        if (hash_function(old_record.key) == hash_function(key)) {

            int previous_position = hash_function(key);
            while (old_record.next != -1 && old_record.key != key) {
                previous_position = old_record.next;
                old_record = read(old_record.next, pFile);
            }

            if (old_record.key != key){
                get<1>(footer)++;
                old_record.next = get<0>(footer);
                update_empty_list(get<0>(footer), 'r', pFile, footer);
                write(previous_position, old_record, pFile);
                write(old_record.next, new_record, pFile);

                record chain_head = read(hash_function(key), pFile);
                chain_head.chain_size++;
                write(hash_function(key), chain_head, pFile);
                get<2>(footer) += chain_head.chain_size;
            }
            else {
                cout << "chave ja existente: " << key << endl;
            }

        }

        else {

            int previous_position = hash_function(old_record.key);
            record old_parent = read(previous_position, pFile);
            while (old_parent.next != hash_function(key)) {
                previous_position = old_parent.next;
                old_parent = read(old_parent.next, pFile);
            }

            old_parent.next = get<0>(footer);
            update_empty_list(get<0>(footer), 'r', pFile, footer);
            write(previous_position, old_parent, pFile);
            write(old_parent.next, old_record, pFile);

            get<1>(footer)++;
            new_record.chain_size = 1;
            get<2>(footer) += new_record.chain_size;
            write(hash_function(key), new_record, pFile);

        }

    }

}

void update_empty_list(int occupied_position, char operation, FILE * pFile, tuple<int, int, int> &footer) {

    record overwritten, adjacent;

    if (operation == 'r') {
        overwritten = read(occupied_position, pFile);
        
        adjacent = read(overwritten.next, pFile);
        adjacent.prev = overwritten.prev;
        write(overwritten.next, adjacent, pFile);
        
        adjacent = read(overwritten.prev, pFile);
        adjacent.next = overwritten.next;
        write(overwritten.prev, adjacent, pFile);

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
            record previous_empty_head = read(get<0>(footer), pFile);
            overwritten.next = get<0>(footer);
            overwritten.prev = previous_empty_head.prev;
            previous_empty_head.prev = occupied_position;
            write(get<0>(footer), previous_empty_head, pFile);
            get<0>(footer) = occupied_position;

            adjacent = read(overwritten.prev, pFile);
            adjacent.next = occupied_position;
            write(overwritten.prev, adjacent, pFile);
        }
        else {
            get<0>(footer) = occupied_position;
            overwritten.next = overwritten.prev = occupied_position;
        }

        write(occupied_position, overwritten, pFile);
    }

}


void find_record(unsigned int key, FILE * pFile) {
    record current_record = read(hash_function(key), pFile);

    while (current_record.next != -1 && current_record.key != key){
        current_record = read(current_record.next, pFile);
    }

    if (current_record.key == key) {
        cout << "chave: " << current_record.key << endl << current_record.content << endl;
    }
    else {
        cout << "chave nao encontrada: " << key << endl;
    }
}

void remove_record(unsigned int key, FILE * pFile, tuple<int, int, int> &footer) {

    int previous_position = hash_function(key), father_position = hash_function(key);
    record sought_record = read(previous_position, pFile);
    while (sought_record.next != -1 && sought_record.key != key) {
        father_position = previous_position;
        previous_position = sought_record.next;
        sought_record = read(sought_record.next, pFile);
    }
    
    if (sought_record.key == key) {

        get<1>(footer)--;
        record chain_head = read(hash_function(key), pFile);
        int chain_size = chain_head.chain_size;

        if (sought_record.next != -1) {
            record sought_child = read(sought_record.next, pFile);
            write(previous_position, sought_child, pFile);
            update_empty_list(sought_record.next, 'i', pFile, footer);
        }

        else {
            record sought_father = read(father_position, pFile);
            sought_father.next = -1;
            write(father_position, sought_father, pFile);
            update_empty_list(previous_position, 'i', pFile, footer);
        }

        chain_head = read(hash_function(key), pFile);
        get<2>(footer) -= chain_size;
        chain_head.chain_size = chain_size - 1;
        write(hash_function(key), chain_head, pFile);

    }
    else {
        cout << "chave nao encontrada: " << key << endl;
    }
}

void print_file(int empty_list_start, FILE * pFile) {

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

void access_average(tuple<int, int, int> &footer) {
    double average = (get<1>(footer) ? (double) get<2>(footer) / get<1>(footer) : 0.0);
    cout << setprecision(1) << fixed << average << endl;
}