#ifndef PROYECTO_PROGRA_3_NGRAMINDEX_H
#define PROYECTO_PROGRA_3_NGRAMINDEX_H

#include <string>
#include <set>
#include <unordered_map>
using namespace std;

class NgramIndex {
    int n;
    unordered_map<string, set<int>> indice;

public:
    explicit NgramIndex(int n_ = 3);

    void insertar(const string& ngrama, int movieId);
    void insertarPalabra(const string& palabra, int movieId);
    set<int> buscar(const string& consulta) const;

    int getN() const { return n; }
    size_t tamano() const { return indice.size(); }
};

#endif