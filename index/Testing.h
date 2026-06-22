#ifndef PROYECTO_PROGRA_3_TESTING_H
#define PROYECTO_PROGRA_3_TESTING_H

#include "Trie.h"
#include "NgramIndex.h"
#include "Indexador.h"
#include "../Catalogo.h"
#include "../Movie.h"
#include <cassert>
#include <iostream>
#include <vector>

inline void testTrieBasico() {
    Trie t;
    t.insertar("batman", 1);
    t.insertar("batman", 2);   // mismo termino, otra peli -> set evita duplicado
    t.insertar("batman", 1);   // duplicado exacto -> ignorado por set
    t.insertar("battle", 3);
    t.insertar("baseball", 4);
    t.insertar("car", 5);

    // buscarExacto: solo si el termino esta marcado como fin
    auto r1 = t.buscarExacto("batman");
    assert(r1.size() == 2);
    assert(r1.count(1) && r1.count(2));

    auto r2 = t.buscarExacto("bat"); // "bat" no fue insertado como palabra
    assert(r2.empty());

    // buscarPorPrefijo: subarbol completo
    auto r3 = t.buscarPorPrefijo("bat");
    // batman(1,2) + battle(3) = {1,2,3}
    assert(r3.size() == 3);
    assert(r3.count(1) && r3.count(2) && r3.count(3));

    auto r4 = t.buscarPorPrefijo("ba");
    // batman(1,2) + battle(3) + baseball(4) = {1,2,3,4}
    assert(r4.size() == 4);

    auto r5 = t.buscarPorPrefijo("xyz");
    assert(r5.empty());

    auto r6 = t.buscarExacto("");
    assert(r6.empty());

    std::cout << "[OK] testTrieBasico" << std::endl;
}

inline void testNgramBasico() {
    NgramIndex idx(3);
    idx.insertarPalabra("barco",  10);
    idx.insertarPalabra("barba",  11);
    idx.insertarPalabra("arco",   12);
    idx.insertarPalabra("rama",   13);

    // "bar" -> ngramas: {"bar"}.
    //   "bar" aparece en barco(10) y barba(11) -> {10,11}
    auto r1 = idx.buscar("bar");
    assert(r1.size() == 2);
    assert(r1.count(10) && r1.count(11));

    // "arco" -> ngramas: {"arc","rco"}.
    //   "arc" en barco(10), arco(12); "rco" en barco(10), arco(12)
    //   interseccion: {10,12}
    auto r2 = idx.buscar("arco");
    assert(r2.size() == 2);
    assert(r2.count(10) && r2.count(12));

    // Palabra mas corta que n -> vacio (caso del Busqueda.txt)
    auto r3 = idx.buscar("ba");
    assert(r3.empty());

    // Sin coincidencias
    auto r4 = idx.buscar("xyz");
    assert(r4.empty());

    std::cout << "[OK] testNgramBasico" << std::endl;
}

inline void testIndexadorConCatalogoMini() {
    // Catalogo de 3 peliculas en memoria, sin tocar el CSV
    Catalogo cat;

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

    Trie trie;
    NgramIndex ngram(3);
    // Stopwords minimas. Deben venir ORDENADAS (tokenizar usa binary_search).
    vector<string> stopwords = {"a","an","and","in","is","of","the","to"};

    Indexador::indexar(cat, trie, ngram, stopwords, /*incluirPlot=*/true);

    // 1) Palabra del plot
    auto r1 = trie.buscarExacto("batman");
    assert(r1.count(0));

    // 2) Prefijo del titulo
    auto r2 = trie.buscarPorPrefijo("inter");
    assert(r2.count(1));

    // 3) Tag de genero compartido
    auto r3 = trie.buscarExacto("tag:crime");
    assert(r3.count(0) && r3.count(2));
    assert(r3.size() == 2);

    // 4) Tag de director (espacios -> _)
    auto r4 = trie.buscarExacto("director:christopher_nolan");
    assert(r4.count(0) && r4.count(1));

    auto r4b = trie.buscarExacto("director:francis_ford_coppola");
    assert(r4b.count(2));

    // 5) Sustring por ngramas en plot
    auto r5 = ngram.buscar("joker");
    assert(r5.count(0));

    // 6) Sustring de fragmento que tambien existe en titulo
    auto r6 = ngram.buscar("knight");
    assert(r6.count(0));

    // 7) Prefijo que toca tags: "tag:" deberia traer TODOS los tags
    auto r7 = trie.buscarPorPrefijo("tag:");
    // action(0), crime(0,2), sci-fi(1), drama(1,2) -> {0,1,2}
    assert(r7.count(0) && r7.count(1) && r7.count(2));

    std::cout << "[OK] testIndexadorConCatalogoMini" << std::endl;

    // Catalogo es dueno de las Movie*: su destructor las libera al salir del scope.
    // (Antes del fix de Paquete 1 habia que hacer delete manual aqui.)
}

inline void correrTestsIndice() {
    std::cout << "=== Tests Paquete 3 (Trie + NgramIndex + Indexador) ===" << std::endl;
    testTrieBasico();
    testNgramBasico();
    testIndexadorConCatalogoMini();
    std::cout << "=== Todos los tests pasaron ===" << std::endl;
}

#endif