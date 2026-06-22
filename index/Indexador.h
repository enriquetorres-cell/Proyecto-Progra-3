#ifndef PROYECTO_PROGRA_3_INDEXADOR_H
#define PROYECTO_PROGRA_3_INDEXADOR_H
 
#include "../Catalogo.h"
#include "Trie.h"
#include "NgramIndex.h"
#include <vector>
#include <string>
 
class Indexador {
public:
    // Indexa todas las peliculas del catalogo en el trie y ngram dados.
    // No los limpia: se acumula sobre lo que ya tengan.
    //
    // Parametros:
    //   catalogo    : fuente de las peliculas (read-only).
    //   trie        : destino para palabras completas y tags.
    //   ngram       : destino para n-gramas (substring).
    //   stopwords   : lista ORDENADA de palabras a descartar
    //                 (tokenizar() hace binary_search sobre ella).
    //   incluirPlot : si false, solo indexa el titulo. true por defecto.
    //                 Plot completo genera trie/ngram muy grandes
    //                 (~30k peliculas x cientos de palabras c/u).
    static void indexar(const Catalogo& catalogo,
                        Trie& trie,
                        NgramIndex& ngram,
                        const std::vector<std::string>& stopwords,
                        bool incluirPlot = true);
};
 
#endif 