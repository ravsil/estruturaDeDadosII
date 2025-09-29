#include "cliente.hpp"
#include <iostream>
#include <string>

using namespace std;

const int M = 6; // quantidade de registros da memoria

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Erro: Nome do arquivo não fornecido." << endl;
        return 1;
    }
    string filename = argv[1];
    cout << "Arquivo: " << filename << endl;
    return 0;
}