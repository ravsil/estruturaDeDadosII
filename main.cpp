#include <iostream>
#include "bmais.cpp"  

using namespace std;

int main() {
    BPlusTree tree(".");

    Cliente c1;
    c1.codCliente = 10;
    strcpy(c1.nome, "Ana");
    c1.idade = 20;
    strcpy(c1.telefone, "1111-1111");

    Cliente c2;
    c2.codCliente = 5;
    strcpy(c2.nome, "Bruno");
    c2.idade = 25;
    strcpy(c2.telefone, "2222-2222");

    Cliente c3;
    c3.codCliente = 30;
    strcpy(c3.nome, "Carla");
    c3.idade = 40;
    strcpy(c3.telefone, "3333-3333");

    tree.insert(c1);
    tree.insert(c2);
    tree.insert(c3);

    cout << "Conteúdo da árvore:\n";
    tree.printAll();

    cout << "\nBuscando chave 10:\n";
    Cliente out;
    if (tree.search(10, out)) {
        cout << "Encontrado: " << out.nome << " (" << out.codCliente << ")\n";
    } else {
        cout << "Não encontrado!\n";
    }

    return 0;
}
