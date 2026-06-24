#include "Ngramindex.h"
#include <algorithm>
#include <iterator>
 
NgramIndex::NgramIndex(int n_) : n(n_) {}
 
void NgramIndex::insertar(const string& ngrama, int movieId) {
    if ((int)ngrama.size() != n) return;
    indice[ngrama].insert(movieId);
}
 
void NgramIndex::insertarPalabra(const string& palabra, int movieId) {
    if ((int)palabra.size() < n) return;
    for (size_t i = 0; i + (size_t)n <= palabra.size(); i++) {
        indice[palabra.substr(i, n)].insert(movieId);
    }
}
 
set<int> NgramIndex::buscar(const string& consulta) const {
    set<int> resultado;
    if ((int)consulta.size() < n) {
        // Consulta mas corta que un ngrama (ej. n=3, "it","up").
        // No se puede formar ningun ngrama -> vacio.
        // El Buscador (Paquete 4) decide si hace fallback.
        return resultado;
    }
 
    bool primero = true;
    for (size_t i = 0; i + (size_t)n <= consulta.size(); i++) {
        string ng = consulta.substr(i, n);
        auto it = indice.find(ng);
        if (it == indice.end()) {
            // Algun ngrama no existe -> la interseccion es vacia, corto.
            return set<int>();
        }
        if (primero) {
            resultado = it->second;
            primero = false;
        } else {
            set<int> temp;
            set_intersection(
                resultado.begin(), resultado.end(),
                it->second.begin(), it->second.end(),
                inserter(temp, temp.begin())
            );
            resultado.swap(temp);
            if (resultado.empty()) return resultado;
        }
    }
    return resultado;
}
 