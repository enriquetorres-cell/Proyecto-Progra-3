#ifndef PROYECTO_PROGRA_3_TRIE_H
#define PROYECTO_PROGRA_3_TRIE_H
 
#include <string>
#include <set>
#include <unordered_map>
using namespace std;
 
class Trie {
    struct Node {
        unordered_map<char, Node*> hijos;
        bool esFin = false;
        set<int> movieIds;   // set: evita duplicados automaticamente
    };
 
    Node* raiz;
 
    // Recolecta los movieIds de todos los nodos esFin del subarbol.
    void recolectarIds(const Node* nodo, set<int>& acumulador) const;
 
    // Destructor recursivo.
    void destruir(Node* nodo);
 
public:
    Trie();
    ~Trie();
 
    // No copia/asigna: simplifica el manejo de memoria.
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
 
    void insertar(const string& palabra, int movieId);
    set<int> buscarExacto(const string& palabra) const;
    set<int> buscarPorPrefijo(const string& prefijo) const;
};
 
#endif 