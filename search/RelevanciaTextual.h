//
// Created by marce on 23/06/2026.
//
#ifndef PROYECTO_PROGRA_3_RELEVANCIATEXTUAL_H
#define PROYECTO_PROGRA_3_RELEVANCIATEXTUAL_H

#include "RankingStrategy.h"

class RelevanciaTextual : public RankingStrategy {
public:
    explicit RelevanciaTextual(const Catalogo& cat);

    vector<int> rankear(const set<int>& ids, const vector<string>& palabras, const Usuario* usuario) const override;};

#endif