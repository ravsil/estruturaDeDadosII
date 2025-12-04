#include <iostream>
#include <vector>
#include "bmais.cpp"
using namespace std;
int main(){
    remove("metadata.bin"); remove("index.bin"); remove("data.bin");
    BPlusTree t(".");
    vector<int> keys = {10,5,30,3,7,15,20,25,40,35,2,8};
    for (int k : keys) {
        Cliente c = BPlusTree::makeCliente(k, string("Nome") + to_string(k), 20 + (k%10), string("0000-") + to_string(k));
        t.insert(c);
    }
    cout << "Após inserções:\n";
    t.printAll();
    cout << "validate: " << (t.validate(true) ? "OK" : "FAIL") << "\n";

    vector<int> deletions = {30,5,10,3,2,40};
    for (int d : deletions) {
        cout << "\nRemovendo " << d << "\n";
        bool ok = t.erase(d);
        cout << "erase("<<d<<") returned " << ok << "\n";
        t.printAll();
        cout << "validate: " << (t.validate(true) ? "OK" : "FAIL") << "\n";
    }

    cout << "\nBuscas finais:\n";
    for (int k : {2,3,5,7,8,10,15,20,25,30,35,40}){
        Cliente out;
        if (t.search(k,out)) cout << "Encontrado "<<k<<" -> "<<out.nome<<"\n";
        else cout << "Nao encontrado "<<k<<"\n";
    }
    return 0;
}
