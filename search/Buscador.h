#ifndef PROYECTO_PROGRA_3_BUSCADOR_H
#define PROYECTO_PROGRA_3_BUSCADOR_H

#include "../Catalogo.h"
#include "../Usuario.h"
#include "../index/Trie.h"
#include "../index/NgramIndex.h"
#include "Ranker.h"
#include <vector>
#include <string>
using namespace std;

// ============================================================
// Buscador (Busqueda.txt)
// ------------------------------------------------------------
// Punto de entrada de la busqueda. Construye un arbol de Query
// a partir de la consulta del usuario (Composite), lo evalua
// sobre los indices y devuelve los ids ya rankeados.
// ============================================================
class Buscador {
    const Catalogo&   catalogo;
    const Trie&       trie;
    const NgramIndex& ngram;
    Ranker            ranker;

public:
    Buscador(const Catalogo& cat, const Trie& trie, const NgramIndex& ngram);

    // 'usuario' puede ser nullptr (busqueda sin sesion activa).
    vector<int> buscar(const string& consulta, const Usuario* usuario) const;
};

#endif
