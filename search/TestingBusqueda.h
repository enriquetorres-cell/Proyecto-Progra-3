#ifndef PROYECTO_PROGRA_3_TESTINGBUSQUEDA_H
#define PROYECTO_PROGRA_3_TESTINGBUSQUEDA_H

#include "Query.h"
#include "Ranker.h"
#include "Buscador.h"
#include "../index/Indexador.h"
#include "../Catalogo.h"
#include "../Movie.h"
#include "../Usuario.h"
#include <cassert>
#include <iostream>
#include <algorithm>
#include <vector>

// Catalogo mini en memoria (no toca el CSV), reusado por todos los tests.
inline void cargarCatalogoMini(Catalogo& cat) {
    auto* m1 = new Movie();
    m1->id = 0; m1->releaseYear = 2008;
    m1->title    = "the dark knight";
    m1->plot     = "Batman fights the Joker in Gotham";
    m1->director = "Christopher Nolan";
    m1->genres   = {"action", "crime"};
    cat.agregar(m1);

    auto* m2 = new Movie();
    m2->id = 1; m2->releaseYear = 2014;
    m2->title    = "interstellar";
    m2->plot     = "Astronauts travel through a wormhole";
    m2->director = "Christopher Nolan";
    m2->genres   = {"sci-fi", "drama"};
    cat.agregar(m2);

    auto* m3 = new Movie();
    m3->id = 2; m3->releaseYear = 1972;
    m3->title    = "the godfather";
    m3->plot     = "Mafia family saga in New York";
    m3->director = "Francis Ford Coppola";
    m3->genres   = {"crime", "drama"};
    cat.agregar(m3);
}

inline vector<string> stopwordsMini() {
    // Deben venir ORDENADAS (tokenizar usa binary_search).
    return {"a","an","and","in","is","of","the","to"};
}

// Helper: true si 'id' esta en el vector de resultados.
inline bool tiene(const vector<int>& v, int id) {
    return find(v.begin(), v.end(), id) != v.end();
}

// 1) Composite: cada hoja se evalua de forma polimorfica.
inline void testQueriesComposite() {
    Catalogo cat; cargarCatalogoMini(cat);
    Trie trie; NgramIndex ngram(3);
    Indexador::indexar(cat, trie, ngram, stopwordsMini(), true);

    // Hoja TermQuery: "batman" (palabra del plot de la peli 0).
    TermQuery t("batman");
    assert(t.evaluar(trie, ngram).count(0));

    // Hoja TagQuery: "tag:crime" -> pelis 0 y 2.
    TagQuery tag("tag:crime");
    auto rTag = tag.evaluar(trie, ngram);
    assert(rTag.count(0) && rTag.count(2) && rTag.size() == 2);

    // Hoja DirectorQuery: "director:christopher_nolan" -> pelis 0 y 1.
    DirectorQuery dir("director:christopher_nolan");
    auto rDir = dir.evaluar(trie, ngram);
    assert(rDir.count(0) && rDir.count(1) && rDir.size() == 2);

    // Compuesto OrQuery: union de "tag:crime" + "director:..." -> {0,1,2}.
    OrQuery orq;
    orq.agregar(new TagQuery("tag:crime"));
    orq.agregar(new DirectorQuery("director:christopher_nolan"));
    auto rOr = orq.evaluar(trie, ngram);
    assert(rOr.count(0) && rOr.count(1) && rOr.count(2));

    std::cout << "[OK] testQueriesComposite" << std::endl;
}

// 2) Ranker: titulo (+3) pesa mas que sinopsis (+1); bonus por genero con like.
inline void testRanker() {
    Catalogo cat; cargarCatalogoMini(cat);
    Ranker ranker(cat);

    // "the godfather" tiene "godfather" en el titulo (peli 2).
    // "interstellar" no contiene esa palabra -> peli 2 debe ir primero.
    set<int> ids = {1, 2};
    auto r = ranker.rankear(ids, {"godfather"}, nullptr);
    assert(!r.empty() && r.front() == 2);

    // Bonus por genero: usuario con like a peli "crime" sube a las "crime".
    Usuario u(1, "test");
    u.darLikeYRegistrarGeneros(0, {"crime"});   // ahora "crime" es favorito
    set<int> ids2 = {1, 2};                      // 2=crime/drama, 1=sci-fi/drama
    auto r2 = ranker.rankear(ids2, {}, &u);
    assert(!r2.empty() && r2.front() == 2);      // 2 comparte "crime" -> +2

    std::cout << "[OK] testRanker" << std::endl;
}

// 3) Buscador end-to-end: parsea, evalua el arbol y rankea.
inline void testBuscadorEndToEnd() {
    Catalogo cat; cargarCatalogoMini(cat);
    Trie trie; NgramIndex ngram(3);
    Indexador::indexar(cat, trie, ngram, stopwordsMini(), true);
    Buscador buscador(cat, trie, ngram);

    // Busqueda normal por palabra del titulo.
    auto r1 = buscador.buscar("interstellar", nullptr);
    assert(tiene(r1, 1));

    // Busqueda por tag de genero.
    auto r2 = buscador.buscar("tag:crime", nullptr);
    assert(tiene(r2, 0) && tiene(r2, 2));

    // Busqueda por director.
    auto r3 = buscador.buscar("director:christopher_nolan", nullptr);
    assert(tiene(r3, 0) && tiene(r3, 1));

    // Consulta de varias palabras -> union de resultados.
    auto r4 = buscador.buscar("batman godfather", nullptr);
    assert(tiene(r4, 0) && tiene(r4, 2));

    // Palabra de 1 letra: se ignora, no rompe la busqueda.
    auto r5 = buscador.buscar("a interstellar", nullptr);
    assert(tiene(r5, 1));

    std::cout << "[OK] testBuscadorEndToEnd" << std::endl;
}

inline void correrTestsBusqueda() {
    std::cout << "=== Tests Paquete 4 (Busqueda + Ranking + Composite) ===" << std::endl;
    testQueriesComposite();
    testRanker();
    testBuscadorEndToEnd();
    std::cout << "=== Todos los tests pasaron ===" << std::endl;
}

#endif
