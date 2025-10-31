#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <climits> 
#include <cstring>
#include "cliente.hpp" 

using namespace std;

struct Particao {
    FILE* file;
    Cliente cliente;
    bool ativa;
};


Particao* particoes;

int getKey(int partition_index) {
    if (!particoes[partition_index].ativa) {
        return INT_MAX;
    }
    return particoes[partition_index].cliente.codCliente;
}

void updateTree(int* tree, int N, int partition_index) {

    int k = partition_index + N;

    k /= 2;

    while (k >= 1) {
        int left_child_part = tree[2 * k];     
        int right_child_part = tree[2 * k + 1];

        if (getKey(left_child_part) <= getKey(right_child_part)) {
            tree[k] = left_child_part;
        } else {
            tree[k] = right_child_part;
        }
        k /= 2; 
    }
}

void mergeFiles(const vector<string>& partitionNames, const string& outputName) {
    int N = partitionNames.size(); // Número de partições
    if (N == 0) {
        cerr << "Nenhuma partição fornecida para intercalar." << endl;
        return;
    }

    particoes = new Particao[N];

    int* tree = new int[2 * N];

    FILE* out = fopen(outputName.c_str(), "wb");
    if (!out) {
        cerr << "Erro ao criar arquivo de saída: " << outputName << endl;
        delete[] particoes;
        delete[] tree;
        return;
    }

    for (int i = 0; i < N; ++i) {
        particoes[i].file = fopen(partitionNames[i].c_str(), "rb");
        if (!particoes[i].file) {
            cerr << "Erro ao abrir partição: " << partitionNames[i] << endl;
            particoes[i].ativa = false;
        } else {
           
            if (fread(&particoes[i].cliente, sizeof(Cliente), 1, particoes[i].file) == 1) {
                particoes[i].ativa = true;
            } else {
               
                particoes[i].ativa = false;
                fclose(particoes[i].file);
                particoes[i].file = nullptr;
            }
        }
    }

    
    for (int i = 0; i < N; ++i) {
        tree[N + i] = i;
    }

    for (int i = N - 1; i > 0; --i) {
        int left_child_part = tree[2 * i];
        int right_child_part = tree[2 * i + 1];

        if (getKey(left_child_part) <= getKey(right_child_part)) {
            tree[i] = left_child_part;
        } else {
            tree[i] = right_child_part;
        }
    }

    int winner_partition = tree[1];

    while (getKey(winner_partition) != INT_MAX) {
        
        fwrite(&particoes[winner_partition].cliente, sizeof(Cliente), 1, out);
        FILE* winnerFile = particoes[winner_partition].file;
        if (winnerFile && 
            fread(&particoes[winner_partition].cliente, sizeof(Cliente), 1, winnerFile) == 1) {
            particoes[winner_partition].ativa = true;
        } else {
            particoes[winner_partition].ativa = false; 
            if (winnerFile) {
                fclose(winnerFile);
                particoes[winner_partition].file = nullptr;
            }
        }

        updateTree(tree, N, winner_partition);
        
        winner_partition = tree[1];
    }

    fclose(out);
    delete[] particoes; 
    delete[] tree;

    cout << "Arquivos intercalados com sucesso em " << outputName << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Uso: " << argv[0] << " <arquivo_saida> <particao1> <particao2> ... <particaoN>" << endl;
        return 1;
    }

    string outputName = argv[1];
    vector<string> partitionNames;
    for (int i = 2; i < argc; ++i) {
        partitionNames.push_back(argv[i]);
    }

    mergeFiles(partitionNames, outputName);

    return 0;
}