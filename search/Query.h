#ifndef PROYECTO_PROGRA_3_QUERY_H
#define PROYECTO_PROGRA_3_QUERY_H

#include "../index/Trie.h"
#include "../index/Ngramindex.h"
#include <set>
#include <vector>
#include <string>
using namespace std;

// ============================================================
// Patron Composite
// ------------------------------------------------------------
// Toda consulta (hoja o compuesta) sabe evaluarse sobre los
// indices y devolver el conjunto de ids que la satisfacen.
// El Buscador arma un arbol de Query y lo evalua polimorficamente.
// ============================================================
class Query {
public:
    virtual set<int> evaluar(const Trie& trie, const NgramIndex& ngram) const = 0;
    virtual ~Query() = default;
};

// Hoja: palabra normal. Segun su longitud decide exacto + prefijo + ngramas.
class TermQuery : public Query {
    string termino;
public:
    explicit TermQuery(const string& t);
    set<int> evaluar(const Trie& trie, const NgramIndex& ngram) const override;
};

// Hoja: tag de genero -> "tag:horror". Busqueda exacta en el trie.
class TagQuery : public Query {
    string clave;   // ya incluye el prefijo "tag:"
public:
    explicit TagQuery(const string& c);
    set<int> evaluar(const Trie& trie, const NgramIndex& ngram) const override;
};

// Hoja: tag de director -> "director:christopher_nolan". Busqueda exacta.
class DirectorQuery : public Query {
    string clave;   // ya incluye el prefijo "director:"
public:
    explicit DirectorQuery(const string& c);
    set<int> evaluar(const Trie& trie, const NgramIndex& ngram) const override;
};

// Compuesto: une (OR) los resultados de todas sus sub-consultas.
class OrQuery : public Query {
    vector<Query*> hijos;   // el OrQuery es dueno de sus hijos
public:
    OrQuery() = default;
    ~OrQuery() override;

    void agregar(Query* q);
    bool vacio() const { return hijos.empty(); }

    set<int> evaluar(const Trie& trie, const NgramIndex& ngram) const override;
};

#endif
