#ifndef PROYECTO_PROGRA_3_QUERY_H
#define PROYECTO_PROGRA_3_QUERY_H

#include "../index/Trie.h"
#include "../index/Ngramindex.h"
#include <set>
#include <vector>
#include <string>
using namespace std;


class Query {
public:
    virtual set<int> evaluar(const Trie& trie, const NgramIndex& ngram) const = 0;
    virtual ~Query() = default;
};

class TermQuery : public Query {
    string termino;
public:
    explicit TermQuery(const string& t);
    set<int> evaluar(const Trie& trie, const NgramIndex& ngram) const override;
};

class TagQuery : public Query {
    string clave;
public:
    explicit TagQuery(const string& c);
    set<int> evaluar(const Trie& trie, const NgramIndex& ngram) const override;
};

class DirectorQuery : public Query {
    string clave;
public:
    explicit DirectorQuery(const string& c);
    set<int> evaluar(const Trie& trie, const NgramIndex& ngram) const override;
};

class OrQuery : public Query {
    vector<Query*> hijos;
public:
    OrQuery() = default;
    ~OrQuery() override;

    void agregar(Query* q);
    bool vacio() const { return hijos.empty(); }

    set<int> evaluar(const Trie& trie, const NgramIndex& ngram) const override;
};

#endif
