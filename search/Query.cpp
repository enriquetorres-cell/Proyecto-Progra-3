#include "Query.h"

// --- Hoja: palabra normal ---
TermQuery::TermQuery(const string& t) : termino(t) {}

set<int> TermQuery::evaluar(const Trie& trie, const NgramIndex& ngram) const {
    set<int> ids;
    // Caso palabra >= 3: exacto + prefijo + ngramas (Busqueda.txt, caso 2).
    if (termino.size() >= 3) {
        set<int> exacto  = trie.buscarExacto(termino);
        set<int> prefijo = trie.buscarPorPrefijo(termino);
        set<int> substr  = ngram.buscar(termino);
        ids.insert(exacto.begin(), exacto.end());
        ids.insert(prefijo.begin(), prefijo.end());
        ids.insert(substr.begin(), substr.end());
    }
    // Caso palabra de 2 letras: solo ngramas (Busqueda.txt, caso 3).
    else if (termino.size() == 2) {
        set<int> substr = ngram.buscar(termino);
        ids.insert(substr.begin(), substr.end());
    }
    // 1 letra o vacio: se ignora (el Buscador ya no deberia crear esta hoja).
    return ids;
}

// --- Hoja: tag de genero ---
TagQuery::TagQuery(const string& c) : clave(c) {}

set<int> TagQuery::evaluar(const Trie& trie, const NgramIndex&) const {
    return trie.buscarExacto(clave);   // "tag:horror" -> match exacto
}

// --- Hoja: tag de director ---
DirectorQuery::DirectorQuery(const string& c) : clave(c) {}

set<int> DirectorQuery::evaluar(const Trie& trie, const NgramIndex&) const {
    return trie.buscarExacto(clave);   // "director:nombre" -> match exacto
}

// --- Compuesto: OR ---
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
