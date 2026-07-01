#include "Query.h"

// --- Hoja: palabra normal ---
TermQuery::TermQuery(const string& t) : termino(t) {}

set<int> TermQuery::evaluar(const Trie& trie, const NgramIndex& ngram) const {
    set<int> ids;
    if (termino.size() >= 3) {
        set<int> exacto  = trie.buscarExacto(termino);
        set<int> prefijo = trie.buscarPorPrefijo(termino);
        set<int> substr  = ngram.buscar(termino);
        ids.insert(exacto.begin(), exacto.end());
        ids.insert(prefijo.begin(), prefijo.end());
        ids.insert(substr.begin(), substr.end());
    }
    else if (termino.size() == 2) {
        set<int> substr = ngram.buscar(termino);
        ids.insert(substr.begin(), substr.end());
    }
    return ids;
}

TagQuery::TagQuery(const string& c) : clave(c) {}

set<int> TagQuery::evaluar(const Trie& trie, const NgramIndex&) const {
    return trie.buscarExacto(clave);
}

DirectorQuery::DirectorQuery(const string& c) : clave(c) {}

set<int> DirectorQuery::evaluar(const Trie& trie, const NgramIndex&) const {
    return trie.buscarExacto(clave);
}

OrQuery::~OrQuery() {
    for (Query* q : hijos) delete q;
}

void OrQuery::agregar(Query* q) {
    if (q != nullptr) hijos.push_back(q);
}

set<int> OrQuery::evaluar(const Trie& trie, const NgramIndex& ngram) const {
    set<int> ids;
    for (const Query* hijo : hijos) {
        set<int> parcial = hijo->evaluar(trie, ngram);
        ids.insert(parcial.begin(), parcial.end());   // union (set deduplica)
    }
    return ids;
}
