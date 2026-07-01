#ifndef PROYECTO_PROGRA_3_AFINIDADCONLIKES_H
#define PROYECTO_PROGRA_3_AFINIDADCONLIKES_H

#include "RankingStrategy.h"
class AfinidadConLikes : public RankingStrategy {
public:
    explicit AfinidadConLikes(const Catalogo& cat);

    vector<int> rankear(const set<int>& ids, const vector<string>& palabras, const Usuario* usuario) const override;};

#endif