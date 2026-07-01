#ifndef PROYECTO_PROGRA_3_BUSCADOR_H
#define PROYECTO_PROGRA_3_BUSCADOR_H

#include "../Catalogo.h"
#include "../Usuario.h"
#include "../index/Trie.h"
#include "../index/Ngramindex.h"
#include "RankingStrategy.h"
#include <vector>
#include <string>
using namespace std;

class Buscador {
    const Catalogo&        catalogo;
    const Trie&            trie;
    const NgramIndex&      ngram;
    const RankingStrategy& strategy;   // Strategy: ranking intercambiable

public:
    Buscador(const Catalogo& cat,
             const Trie& trie,
             const NgramIndex& ngram,
             const RankingStrategy& strategy);

    // 'usuario' puede ser nullptr (busqueda sin sesion activa).
    vector<int> buscar(const string& consulta, const Usuario* usuario) const;
};

#endif