#ifndef PROYECTO_PROGRA_3_INDEXADOR_H
#define PROYECTO_PROGRA_3_INDEXADOR_H
 
#include "../Catalogo.h"
#include "Trie.h"
#include "Ngramindex.h"
#include <vector>
#include <string>
 
class Indexador {
public:

    static void indexar(const Catalogo& catalogo,
                        Trie& trie,
                        NgramIndex& ngram,
                        const std::vector<std::string>& stopwords,
                        bool incluirPlot = true);
};
 
#endif 