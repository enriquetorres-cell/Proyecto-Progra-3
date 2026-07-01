//
// Created by marce on 1/07/2026.
//

#ifndef PROYECTO_PROGRA_3_RANKINGUTILS_H
#define PROYECTO_PROGRA_3_RANKINGUTILS_H
#include <vector>
#include <utility>
#include <algorithm>
using namespace std;

template <typename Score, typename Comparador>
vector<int> ordenarPorPuntaje(vector<pair<int, Score>> candidatos, Comparador comparador, int topN = -1) {
    stable_sort(candidatos.begin(), candidatos.end(), comparador);
    size_t limite = candidatos.size();
    if (topN >= 0 && (size_t)topN < limite) limite = (size_t)topN;
    vector<int> resultado;
    resultado.reserve(limite);
    for (size_t i = 0; i < limite; i++) resultado.push_back(candidatos[i].first);
    return resultado;
}
#endif //PROYECTO_PROGRA_3_RANKINGUTILS_H