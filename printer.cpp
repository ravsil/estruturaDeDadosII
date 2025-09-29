#include "cliente.hpp"
#include <iostream>
#include <string>
#include <iomanip>

using namespace std;

// função para imprimir o conteúdo de um arquivo de clientes
void printFile(const string &filename)
{
    FILE *file = fopen(filename.c_str(), "rb");
    if (!file)
    {
        cerr << "Erro ao abrir o arquivo: " << filename << endl;
        return;
    }
    Cliente c;
    cout << "Código    | Nome                 | Data de Nascimento" << endl;
    cout << "-----------------------------------------------------" << endl;
    while (fread(&c, sizeof(Cliente), 1, file) == 1)
    {
        cout << left << setw(9) << c.codCliente << " | "
             << setw(20) << c.nome << " | "
             << c.dataNascimento << endl;
    }
    fclose(file);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Erro: argumentos inválidos." << endl;
        return 1;
    }
    printFile(argv[1]);
    return 0;
}