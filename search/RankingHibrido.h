#ifndef PROYECTO_PROGRA_3_RANKINGHIBRIDO_H
#define PROYECTO_PROGRA_3_RANKINGHIBRIDO_H

#include "../Catalogo.h"
#include "../Usuario.h"
#include <set>
#include <vector>
#include <string>

#include "RankingStrategy.h"
using namespace std;

// ============================================================
// Ranker (Ranking.txt)
// ------------------------------------------------------------
// Ordena los ids resultantes de una busqueda por relevancia:
//   +3 si la palabra aparece en el titulo
//   +2 si aparece en el director
//   +1 si aparece en la sinopsis (plot)
//   +2 extra si la pelicula comparte genero con los likes del usuario
// ============================================================
class RankingHibrido: public RankingStrategy {
public:
    explicit RankingHibrido(const Catalogo& cat);
    vector<int> rankear(const set<int>& ids, const vector<string>& palabras, const Usuario* usuario) const override;};

#endif
