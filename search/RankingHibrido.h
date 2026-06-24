#ifndef PROYECTO_PROGRA_3_RANKINGHIBRIDO_H
#define PROYECTO_PROGRA_3_RANKINGHIBRIDO_H

#include "../Catalogo.h"
#include "../Usuario.h"
#include <set>
#include <vector>
#include <string>

#include "RankingStrategy.h"
using namespace std;


class RankingHibrido: public RankingStrategy {
public:
    explicit RankingHibrido(const Catalogo& cat);
    vector<int> rankear(const set<int>& ids, const vector<string>& palabras, const Usuario* usuario) const override;};

#endif
