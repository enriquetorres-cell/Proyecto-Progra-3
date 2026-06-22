#include "Trie.h"
 
Trie::Trie() {
    raiz = new Node();
}
 
Trie::~Trie() {
    destruir(raiz);
    raiz = nullptr;
}
 
void Trie::destruir(Node* nodo) {
    if (nodo == nullptr) return;
    for (auto& par : nodo->hijos) {
        destruir(par.second);
    }
    delete nodo;
}
 
void Trie::insertar(const string& palabra, int movieId) {
    if (palabra.empty()) return;
 
    Node* actual = raiz;
    for (char c : palabra) {
        auto it = actual->hijos.find(c);
        if (it == actual->hijos.end()) {
            Node* nuevo = new Node();
            actual->hijos[c] = nuevo;
            actual = nuevo;
        } else {
            actual = it->second;
        }
    }
    actual->esFin = true;
    actual->movieIds.insert(movieId); // set: no duplicados
}
 
set<int> Trie::buscarExacto(const string& palabra) const {
    set<int> resultado;
    if (palabra.empty()) return resultado;
 
    const Node* actual = raiz;
    for (char c : palabra) {
        auto it = actual->hijos.find(c);
        if (it == actual->hijos.end()) return resultado; // vacio
        actual = it->second;
    }
    if (actual->esFin) {
        resultado = actual->movieIds;
    }
    return resultado;
}
 
set<int> Trie::buscarPorPrefijo(const string& prefijo) const {
    set<int> resultado;
    if (prefijo.empty()) return resultado;
 
    // 1) Navegar hasta el nodo de fin de prefijo
    const Node* actual = raiz;
    for (char c : prefijo) {
        auto it = actual->hijos.find(c);
        if (it == actual->hijos.end()) return resultado; // vacio
        actual = it->second;
    }
    // 2) DFS desde ahi acumulando todos los movieIds
    recolectarIds(actual, resultado);
    return resultado;
}
 
void Trie::recolectarIds(const Node* nodo, set<int>& acumulador) const {
    if (nodo == nullptr) return;
    if (nodo->esFin) {
        acumulador.insert(nodo->movieIds.begin(), nodo->movieIds.end());
    }
    for (const auto& par : nodo->hijos) {
        recolectarIds(par.second, acumulador);
    }
}