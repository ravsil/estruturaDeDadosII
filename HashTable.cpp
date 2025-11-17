#include "HashTable.hpp"
#include <iostream>
#include <stdexcept>
#include <chrono>

using namespace std;

HashTable::HashTable(int size, std::string baseName) {
    if (size <= 0) {
        throw std::runtime_error("Tamanho da tabela deve ser positivo.");
    }
    table_size = size;
    table = new HashSlot[table_size];
    num_items = 0;
    collisions_insert = 0;
    collisions_search = 0;
    name = baseName;
}

HashTable::~HashTable() {
    delete[] table;
}

int HashTable::hash1(int key) {
    return (unsigned int)key % table_size;
}

int HashTable::hash2(int key) {
    int h = (unsigned int)key % (table_size - 1);
    return 1 + h;
}

double HashTable::getLoadFactor() {
    return (double)num_items / table_size;
}

long long HashTable::getInsertCollisions() {
    return collisions_insert;
}

long long HashTable::getSearchCollisions() {
    return collisions_search;
}

std::string HashTable::getName() {
    return name;
}

void HashTable::resetMetrics() {
    collisions_insert = 0;
    collisions_search = 0;
}

bool HashTable::insert(const Cliente& c) {
    if (getLoadFactor() >= 1.0) {
        return false;
    }

    int key = c.codCliente;
    int i = 0;
    long long collisions_for_this_insert = 0;

    while (i < table_size) {
        int index = probe(key, i);
        HashSlot& slot = table[index];

        if (slot.status == EMPTY || slot.status == DELETED) {
            slot.cliente = c;
            slot.status = OCCUPIED;
            num_items++;
            collisions_insert += collisions_for_this_insert;
            return true;
        }

        if (slot.cliente.codCliente == key) {
            slot.cliente = c;
            collisions_insert += collisions_for_this_insert;
            return true;
        }

        collisions_for_this_insert++;
        i++;
    }

    return false;
}

Cliente* HashTable::search(int key, double& time_taken) {
    auto start = std::chrono::high_resolution_clock::now();

    int i = 0;
    long long collisions_for_this_search = 0;

    while (i < table_size) {
        int index = probe(key, i);
        HashSlot& slot = table[index];

        if (slot.status == EMPTY) {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = end - start;
            time_taken = diff.count();
            
            collisions_search += collisions_for_this_search;
            return nullptr;
        }

        if (slot.status == DELETED) {
            collisions_for_this_search++;
            i++;
            continue;
        }

        if (slot.status == OCCUPIED && slot.cliente.codCliente == key) {
            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> diff = end - start;
            time_taken = diff.count();
            
            collisions_search += collisions_for_this_search;
            return &(slot.cliente);
        }

        collisions_for_this_search++;
        i++;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    time_taken = diff.count();

    collisions_search += collisions_for_this_search;
    return nullptr;
}

bool HashTable::remove(int key) {
    int i = 0;
    while (i < table_size) {
        int index = probe(key, i);
        HashSlot& slot = table[index];

        if (slot.status == EMPTY) {
            return false;
        }

        if (slot.status == OCCUPIED && slot.cliente.codCliente == key) {
            slot.status = DELETED;
            slot.cliente.codCliente = -2; 
            num_items--;
            return true;
        }
        i++;
    }
    return false;
}

LinearProbingHashTable::LinearProbingHashTable(int size)
    : HashTable(size, "Tentativa Linear") {}

int LinearProbingHashTable::probe(int key, int i) {
    return (hash1(key) + i) % table_size;
}

QuadraticProbingHashTable::QuadraticProbingHashTable(int size, int c1, int c2)
    : HashTable(size, "Tentativa Quadratica"), c1(c1), c2(c2) {}

int QuadraticProbingHashTable::probe(int key, int i) {
    long long i_squared = (long long)i * i;
    long long result = (hash1(key) + (long long)c1 * i + (long long)c2 * i_squared);
    return (int)(result % table_size);
}


DoubleHashingHashTable::DoubleHashingHashTable(int size)
    : HashTable(size, "Dispersao Dupla") {}

int DoubleHashingHashTable::probe(int key, int i) {
    long long result = (long long)hash1(key) + (long long)i * hash2(key);
    return (int)(result % table_size);
}