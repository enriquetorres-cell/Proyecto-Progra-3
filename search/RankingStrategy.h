//
// Created by marce on 23/06/2026.
//

#ifndef PROYECTO_PROGRA_3_RANKINGSTRATEGY_H
#define PROYECTO_PROGRA_3_RANKINGSTRATEGY_H
#include "../Catalogo.h"
#include "../Usuario.h"
#include <set>
#include <vector>
#include <string>
using namespace std;

class RankingStrategy {
protected:
    const Catalogo& catalogo;
public:
    explicit RankingStrategy(const Catalogo& cat) : catalogo(cat) {}
    virtual ~RankingStrategy() = default;

    virtual vector<int> rankear(const set<int>& ids, const vector<string>& palabras, const Usuario* usuario) const = 0;};


#endif //PROYECTO_PROGRA_3_RANKINGSTRATEGY_H