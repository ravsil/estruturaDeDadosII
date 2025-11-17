#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <stdexcept>
#include <algorithm>
#include <cstring>

#include "cliente.hpp"
#include "HashTable.hpp"

using namespace std;

vector<Cliente> gerar_clientes(int N) {
    vector<Cliente> clientes;
    vector<int> chaves;
    for (int i = 0; i < N * 2; ++i) {
        chaves.push_back(i + 1);
    }
    
    std::random_shuffle(chaves.begin(), chaves.end());

    for (int i = 0; i < N; ++i) {
        Cliente c;
        c.codCliente = chaves[i];
        
        snprintf(c.nome, sizeof(c.nome), "Cliente%04d", c.codCliente);
        snprintf(c.dataNascimento, sizeof(c.dataNascimento), "%02d/%02d/%04d",
                 1 + rand() % 28, 1 + rand() % 12, 1950 + rand() % 70);
        
        clientes.push_back(c);
    }
    return clientes;
}

void rodar_teste(HashTable& ht, const vector<Cliente>& clientes_para_inserir, const vector<int>& chaves_para_buscar) {
    ht.resetMetrics();

    for (const auto& cliente : clientes_para_inserir) {
        ht.insert(cliente);
    }

    double tempo_total_busca = 0.0;
    int buscas_sucedidas = 0;
    
    for (const auto& chave : chaves_para_buscar) {
        double tempo_gasto = 0.0;
        Cliente* encontrado = ht.search(chave, tempo_gasto);
        
        if (encontrado != nullptr) {
            tempo_total_busca += tempo_gasto;
            buscas_sucedidas++;
        }
    }
    
    double tempo_medio_busca = (buscas_sucedidas > 0) ? (tempo_total_busca / buscas_sucedidas) : 0.0;

    cout << "  | " << left << setw(22) << ht.getName();
    cout << " | " << right << setw(10) << ht.getInsertCollisions();
    cout << " | " << right << setw(10) << ht.getSearchCollisions();
    cout << " | " << right << setw(18) << fixed << setprecision(4) << (tempo_medio_busca * 1e6) << " |" << endl;
}

int main() {
    srand(time(nullptr));

    vector<int> tamanhos_tabela = {1000, 100000, 1000000};
    vector<double> fatores_carga = {0.7, 0.8, 0.9};

    cout << "Iniciando Experimento de Tabela Hash (Enderecamento Aberto)" << endl;
    cout << "==========================================================================" << endl;
    
    for (int M : tamanhos_tabela) {
        for (double alpha : fatores_carga) {
            
            int N = (int)(M * alpha);
            
            cout << "\n--- Testando Cenario ---" << endl;
            cout << "Tamanho Tabela (M): " << M << endl;
            cout << "Fator de Carga (a): " << alpha << endl;
            cout << "Num. Itens (N):     " << N << endl;
            
            vector<Cliente> clientes = gerar_clientes(N);
            vector<int> chaves_para_buscar;
            for(const auto& c : clientes) {
                chaves_para_buscar.push_back(c.codCliente);
            }
            std::random_shuffle(chaves_para_buscar.begin(), chaves_para_buscar.end());

            cout << "--------------------------------------------------------------------------" << endl;
            cout << "  | Metodo                   | Colisoes | Colisoes | Tempo Medio Busca  |" << endl;
            cout << "  |                          | (Inserir)| (Buscar) | (microsegundos)    |" << endl;
            cout << "  +--------------------------+----------+----------+--------------------+-" << endl;

            try {
                LinearProbingHashTable ht_linear(M);
                rodar_teste(ht_linear, clientes, chaves_para_buscar);

                QuadraticProbingHashTable ht_quad(M);
                rodar_teste(ht_quad, clientes, chaves_para_buscar);
                
                DoubleHashingHashTable ht_double(M);
                rodar_teste(ht_double, clientes, chaves_para_buscar);

            } catch (const std::runtime_error& e) {
                cerr << "Erro ao inicializar tabelas: " << e.what() << endl;
                continue;
            }

            cout << "  +--------------------------+----------+----------+--------------------+-" << endl;
        }
    }

    cout << "\n==========================================================================" << endl;
    cout << "Experimento Concluido." << endl;

    return 0;
}