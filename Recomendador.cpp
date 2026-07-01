//
// Created by marce on 23/06/2026.
//

#include "Recomendador.h"
#include <algorithm>
#include <utility>
#include "RankingUtils.h"

Recomendador::Recomendador(const Catalogo& cat) : catalogo(cat) {}

map<string, int> Recomendador::calcularPesosPorGenero(const UsuarioHistorial& h) const {
    map<string, int> pesos;

    const map<string, int>& favoritos = h.getGenerosFavoritos();
    for (const auto& par : favoritos) { //caslculamos segun favoritos
        pesos[par.first] += par.second * pesoLike;}

    const vector<int>& verMasTarde = h.getWatchLater();   //calculamos segun ver mas tarde
    for (int id : verMasTarde) {
        const Movie* peli = catalogo.getById(id);
        if (peli == nullptr) continue;
        for (const string& g : peli->genres) {
            pesos[g] += pesoWatchLater;}}

    const vector<int>& recientes = h.getVistasRecientes(); //segun seleccion de una pelicula a partir de una busqueda
    for (int id : recientes) {
        const Movie* peli = catalogo.getById(id);
        if (peli == nullptr) continue;
        for (const string& g : peli->genres) {
            pesos[g] += pesoVista;}}
    return pesos;}

bool Recomendador::yaVista(int movieId, const UsuarioHistorial& h) const {
    return h.tieneLike(movieId) || h.estaEnVerMasTarde(movieId);}

int Recomendador::puntajePelicula(const Movie* peli, const map<string, int>& pesos) const {
    int score = 0;
    for (const string& g : peli->genres) {
        auto it = pesos.find(g);
        if (it != pesos.end()) score += it->second;}
    return score;}

vector<int> Recomendador::recomendar(const UsuarioHistorial& historial, int topN) const {
    map<string, int> pesos = calcularPesosPorGenero(historial);
    if (pesos.empty()) return  peliculasMasRecientes(topN);
    vector<pair<int, int>> candidatos;
    candidatos.reserve(catalogo.size());
    for (Movie* peli : catalogo.getMovies()) {
        if (peli == nullptr) continue;
        if (yaVista(peli->id, historial)) continue;
        if (peli->genres.empty()) continue;
        int score = puntajePelicula(peli, pesos);
        if (score > 0) {
            candidatos.push_back({peli->id, score});}}
    return ordenarPorPuntaje(std::move(candidatos),
        [](const pair<int,int>& a, const pair<int,int>& b) {
            if (a.second != b.second) return a.second > b.second;
            return a.first < b.first;}, topN);
}

vector<int> Recomendador::peliculasMasRecientes(int topN) const {
    vector<pair<int, int>> porAnio;   // id, anio
    porAnio.reserve(catalogo.size());

    for (Movie* peli : catalogo.getMovies()) {
        if (peli == nullptr || peli->releaseYear == 0) continue;
        if (peli->genres.empty()) continue;
        porAnio.push_back({peli->id, peli->releaseYear});}

    return ordenarPorPuntaje(std::move(porAnio),
        [](const pair<int,int>& a, const pair<int,int>& b) {
            return a.second > b.second;
        }, topN);}
