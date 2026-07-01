//
// Created by marce on 1/07/2026.
//

#ifndef PROYECTO_PROGRA_3_RANKINGCONETIQUETAS_H
#define PROYECTO_PROGRA_3_RANKINGCONETIQUETAS_H
#include "RankingStrategy.h"
#include "../UsuarioHistorial.h"
#include <algorithm>
#include <unordered_map>
using namespace std;


class RankingConEtiquetas : public RankingStrategy {
    const RankingStrategy& interna;
    mutable unordered_map<int, string> etiquetas;

    static string calcularEtiqueta(int movieId, const UsuarioHistorial* hist) {
        string texto;
        if (hist->tieneLike(movieId))         texto += "[Con like] ";
        if (hist->estaEnVerMasTarde(movieId)) texto += "[Ver mas tarde] ";
        const vector<int>& vistas = hist->getVistasRecientes();
        if (find(vistas.begin(), vistas.end(), movieId) != vistas.end())
            texto += "[Ya vista] ";
        return texto;}

public:
    RankingConEtiquetas(const Catalogo& cat, const RankingStrategy& estrategia)
        : RankingStrategy(cat), interna(estrategia) {}

    vector<int> rankear(const set<int>& ids, const vector<string>& palabras, const Usuario* usuario) const override {
        vector<int> resultado = interna.rankear(ids, palabras, usuario);

        etiquetas.clear();
        if (usuario != nullptr) {
            const UsuarioHistorial* hist = usuario->getHistorial();
            for (int id : resultado) {
                string e = calcularEtiqueta(id, hist);
                if (!e.empty()) etiquetas[id] = e;}}
        return resultado;}


    string getEtiqueta(int movieId) const {
        auto it = etiquetas.find(movieId);
        return (it != etiquetas.end()) ? it->second : "";}
};

#endif