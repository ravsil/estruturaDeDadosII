#ifndef HASHTABLE_HPP
#define HASHTABLE_HPP

#include <string>
#include <vector>
#include "cliente.hpp"

enum SlotStatus {
    EMPTY,
    OCCUPIED,
    DELETED
};

struct HashSlot {
    Cliente cliente;
    SlotStatus status;

    HashSlot() {
        cliente.codCliente = -1; 
        status = EMPTY;
    }
};

class HashTable {
protected:
    HashSlot* table;
    int table_size;
    int num_items;
    std::string name;

    long long collisions_insert;
    long long collisions_search;

    int hash1(int key);

    int hash2(int key);

    virtual int probe(int key, int i) = 0;

public:
    HashTable(int size, std::string name);
    virtual ~HashTable();

    bool insert(const Cliente& c);
    Cliente* search(int key, double& time_taken);
    bool remove(int key);

    double getLoadFactor();
    long long getInsertCollisions();
    long long getSearchCollisions();
    std::string getName();
    void resetMetrics();
};

class LinearProbingHashTable : public HashTable {
public:
    LinearProbingHashTable(int size);
protected:
    int probe(int key, int i) override;
};


class QuadraticProbingHashTable : public HashTable {
private:
    int c1, c2;
public:
    QuadraticProbingHashTable(int size, int c1 = 1, int c2 = 1);
protected:
    int probe(int key, int i) override;
};


class DoubleHashingHashTable : public HashTable {
public:
    DoubleHashingHashTable(int size);
protected:
    int probe(int key, int i) override;
};

#endif