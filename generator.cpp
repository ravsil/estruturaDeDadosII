#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

#include "cliente.hpp"
#include <cstring>

// função para gerar um arquivo com N clientes aleatórios
void genFile(const string &filename, int N, int minValue = 0, int maxValue = 9999)
{
    FILE *file = fopen(filename.c_str(), "wb");
    if (!file)
    {
        cerr << "Erro ao criar o arquivo binário: " << filename << endl;
        return;
    }
    srand(time(nullptr));
    for (int i = 0; i < N; ++i)
    {
        Cliente c;
        c.codCliente = minValue + rand() % (maxValue - minValue + 1);
        snprintf(c.nome, sizeof(c.nome), "Cliente%04d", c.codCliente);
        snprintf(c.dataNascimento, sizeof(c.dataNascimento), "%02d/%02d/%04d",
                 1 + rand() % 28, 1 + rand() % 12, 1950 + rand() % 70);
        fwrite(&c, sizeof(Cliente), 1, file);
    }
    fclose(file);
    cout << "Arquivo binário '" << filename << "' gerado com " << N << " clientes.\n";
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cerr << "Uso: " << argv[0] << " <arquivo> <N>\n";
        return 1;
    }
    string filename = argv[1];
    int N = stoi(argv[2]);
    genFile(filename, N);
    return 0;
}
