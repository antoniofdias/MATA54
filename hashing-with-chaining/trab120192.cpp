#include <bits/stdc++.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>

using namespace std;

struct registro {
    unsigned int key;
    char content[21];
    bool empty;
    int prev;
    int next;
    int chain_size;
};

void insere_registro(int chave, string conteudo);
void consulta_registro(int chave);
void remove_registro(int chave);
void imprime_arquivo();
void media_acessos();
void inicia_arquivo();
int offset (int chave);
void remove_empty_position(registro antigo, int position);
void create_empty_position(registro antigo, int position);

int TAMANHO_ARQUIVO = 11;
FILE * pFile;

int main() {
	ios::sync_with_stdio(false);
	cin.tie(0);
	cout.tie(0);
    if (access("file.bin", F_OK) == -1) inicia_arquivo();
    pFile = fopen("file.bin", "r+");
    int chave;
    string conteudo;
    char op;
    while (cin >> op, op != 'e') {
        switch (op) {
            case 'i':
                cin >> chave;
                cin.ignore();
                getline(cin, conteudo);
                insere_registro(chave, conteudo);
                break;
            case 'c':
                cin >> chave;
                consulta_registro(chave);
                break;
            case 'r':
                cin >> chave;
                remove_registro(chave);
                break;
            case 'p':
                imprime_arquivo();
                break;
            case 'm':
                media_acessos();
                break;
            default:
                break;
        }
    }
    fclose(pFile);
}

int offset (int chave) {
    return (chave % TAMANHO_ARQUIVO);
}

void inicia_arquivo() {
    pFile = fopen("file.bin", "w");
    registro novo_registro;
    for (int i = 0; i < TAMANHO_ARQUIVO; i++) {
        novo_registro.key = i, novo_registro.empty = true;
        novo_registro.prev = (i + TAMANHO_ARQUIVO - 1) % TAMANHO_ARQUIVO;
        novo_registro.next = (i + 1) % TAMANHO_ARQUIVO;
        fwrite(&novo_registro, sizeof(struct registro), 1, pFile);
    }
    int empty_list_start = 0;
    fwrite(&empty_list_start, sizeof(int), 1, pFile);
    fclose(pFile);
}

void remove_empty_position(registro antigo, int position) {
    registro overwritten;

    fseek(pFile, sizeof(struct registro) * offset(antigo.next), SEEK_SET);
    fread(&overwritten, sizeof(struct registro), 1, pFile);
    overwritten.prev = antigo.prev;
    fseek(pFile, sizeof(struct registro) * offset(antigo.next), SEEK_SET);
    fwrite(&overwritten, sizeof(struct registro), 1, pFile);

    fseek(pFile, sizeof(struct registro) * offset(antigo.prev), SEEK_SET);
    fread(&overwritten, sizeof(struct registro), 1, pFile);
    overwritten.next = antigo.next;
    fseek(pFile, sizeof(struct registro) * offset(antigo.prev), SEEK_SET);
    fwrite(&overwritten, sizeof(struct registro), 1, pFile);

    int empty_list_start;
    fseek(pFile, -sizeof(int), SEEK_END);
    fread(&empty_list_start, sizeof(int), 1, pFile);
    if (empty_list_start == position) {
        fseek(pFile, -sizeof(int), SEEK_END);
        empty_list_start = antigo.next;
        fwrite(&empty_list_start, sizeof(int), 1, pFile);
    }
}

void insere_registro(int chave, string conteudo) {
    registro novo_registro, velho_registro;
    novo_registro.key = chave;
    for (unsigned int i = 0; i < conteudo.size(); i++) {
        novo_registro.content[i] = conteudo[i];
    }
    novo_registro.content[conteudo.size()] = '\0';
    novo_registro.empty = false;
    novo_registro.next = novo_registro.prev = -1;
    fseek(pFile, sizeof(registro) * offset(chave), SEEK_SET);
    fread(&velho_registro, sizeof(struct registro), 1, pFile);
    if (velho_registro.empty) {
        remove_empty_position(velho_registro, offset(chave));
        fseek(pFile, sizeof(registro) * offset(chave), SEEK_SET);
        fwrite(&novo_registro, sizeof(struct registro), 1, pFile);
    }
    else if (velho_registro.key == offset(chave)) {
        int position = offset(chave);
        while (!velho_registro.key != chave && velho_registro.next != -1) {
            position = velho_registro.next;
            fseek(pFile, sizeof(registro) * velho_registro.next, SEEK_SET);
            fread(&velho_registro, sizeof(struct registro), 1, pFile);
        }
        if (velho_registro.key == chave) {
            cout << "chave ja existente: " << chave << endl;
        }
        else {
            int empty_list_start;
            fseek(pFile, -sizeof(int), SEEK_END);
            fread(&empty_list_start, sizeof(int), 1, pFile);
            velho_registro.next = empty_list_start;
            fseek(pFile, sizeof(struct registro) * position, SEEK_SET);
            fwrite(&velho_registro, sizeof(struct registro), 1, pFile);
            fseek(pFile, sizeof(struct registro) * empty_list_start, SEEK_SET);
            fwrite(&novo_registro, sizeof(struct registro), 1, pFile);
        }
    }
    else if (false) {
        //TODO reposition
    }
}


void consulta_registro(int chave) {
    registro buffer;
    int count = 0;
    fseek(pFile, sizeof(struct registro) * offset(chave), SEEK_SET);
    fread(&buffer, sizeof(struct registro), 1, pFile);
    while (!buffer.empty && buffer.key != chave && count < TAMANHO_ARQUIVO) {
        fseek(pFile, sizeof(registro) * buffer.next, SEEK_SET);
        count++;
    }
    if (buffer.key == chave) {
        cout << "chave: " << buffer.key << endl;
        for (int i = 0; i < 21 && buffer.content[i] != '\0'; i++) {
            cout << buffer.content[i];
        }
        cout << endl;
    }
    else {
        cout << "chave nao encontrada: " << chave << endl;
    }
}

void remove_registro(int chave) {

}

void imprime_arquivo() {
    registro buffer;
    fseek(pFile, 0, SEEK_SET);
    for (int i = 0; i < TAMANHO_ARQUIVO; i++) {
        fread(&buffer, sizeof(struct registro), 1, pFile);
        cout << i << ": ";
        if (buffer.empty) {
            cout << "apontador1: " << buffer.prev << " apontador2: " << buffer.next << endl;
        }
        else {
            cout << buffer.key << ' ';
            for (int j = 0; j < 21 && buffer.content[j] != '\0'; j++) {
                cout << buffer.content[j];
            }
            cout << ' ' << (buffer.next != -1 ? to_string(buffer.next) : "nulo") << endl;
        }
    }
    cout << "posicao inicial da lista de posicoes vazias: ";
    int empty_list_start;
    fseek(pFile, -sizeof(int), SEEK_END);
    fread(&empty_list_start, sizeof(int), 1, pFile);
    cout << empty_list_start << endl;
}

void media_acessos() {

}
