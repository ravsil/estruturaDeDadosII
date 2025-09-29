#include "cliente.hpp"
#include <iostream>
#include <string>

using namespace std;

const int M = 6;         // quantidade de registros da memória
const int RESERVOIR = M; // tamanho do reservatório

// função para executar a seleção natural
void naturalSelection(const string &filename)
{
    FILE *in = fopen(filename.c_str(), "rb");
    if (!in)
    {
        cerr << "Erro ao abrir o arquivo:" << filename << endl;
        return;
    }

    Cliente memory[M];
    int nMem = 0;
    while (nMem < M && fread(&memory[nMem], sizeof(Cliente), 1, in) == 1)
    {
        nMem++;
    }

    Cliente reservatorio[RESERVOIR];
    int nReserv = 0;
    int particao = 1;

    while (nMem > 0)
    {
        string outName = "p" + to_string(particao) + ".dat";
        FILE *out = fopen(outName.c_str(), "wb");

        int lastKey = -1;
        nReserv = 0;

        while (nMem > 0)
        {
            int firstIndex = 0;
            for (int i = 1; i < nMem; i++)
            {
                if (memory[i].codCliente < memory[firstIndex].codCliente)
                    firstIndex = i;
            }
            Cliente first = memory[firstIndex];
            fwrite(&first, sizeof(Cliente), 1, out);
            lastKey = first.codCliente;

            Cliente next;
            if (fread(&next, sizeof(Cliente), 1, in) == 1)
            {
                if (next.codCliente < lastKey)
                {
                    if (nReserv < RESERVOIR)
                    {
                        reservatorio[nReserv++] = next;
                        for (int j = firstIndex; j < nMem - 1; j++)
                            memory[j] = memory[j + 1];
                        nMem--;
                    }
                    else
                    {
                        fseek(in, -static_cast<long>(sizeof(Cliente)), SEEK_CUR);
                        break;
                    }
                }
                else
                {
                    memory[firstIndex] = next;
                }
            }
            else
            {
                for (int j = firstIndex; j < nMem - 1; j++)
                    memory[j] = memory[j + 1];
                nMem--;
            }
        }
        fclose(out);

        for (int i = 0; i < nReserv; i++)
            memory[i] = reservatorio[i];
        nMem = nReserv;
        particao++;
    }
    fclose(in);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Erro: argumentos inválidos." << endl;
        return 1;
    }
    naturalSelection(argv[1]);
    return 0;
}