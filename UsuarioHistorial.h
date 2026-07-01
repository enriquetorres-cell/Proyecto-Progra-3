//
// Created by Santiago Davila on 20/06/26.
//

#ifndef REPO_PROGRA3_USUARIOHISTORIAL_H
#define REPO_PROGRA3_USUARIOHISTORIAL_H
#include <set>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
using namespace std;

class UsuarioHistorial{
    set<int> likes;
    vector<int> watchLater;
    map<string, int> generosFavoritos;
    vector<int> vistasRecientes;
    static const size_t MAX_RECIENTES = 20;
public:
    UsuarioHistorial() = default;

    void agregarLike(int movieId) {likes.insert(movieId);}

    void quitarLike(int movieId) {likes.erase(movieId);}

    bool tieneLike(int movieId) const {
        return likes.find(movieId) != likes.end();}

    bool estaEnVerMasTarde(int movieId) const {
        return find(watchLater.begin(), watchLater.end(), movieId) != watchLater.end();}

    void agregarVerMasTarde(int movieId) {
        if (!estaEnVerMasTarde(movieId))  watchLater.push_back(movieId);}

    void quitarVerMasTarde(int movieId) {
        watchLater.erase(
            remove(watchLater.begin(), watchLater.end(), movieId),
            watchLater.end());}

    void registrarGenero(const string& genero) {generosFavoritos[genero]++;}

    void registrarGeneros(const vector<string>& generos) {
        for (const string& genero : generos) {
            registrarGenero(genero);}}

    const set<int>& getLikes() const {
        return likes;}

    const vector<int>& getWatchLater() const {
        return watchLater;}

    const map<string, int>& getGenerosFavoritos() const {
        return generosFavoritos;}

    void limpiarHistorial() {
        likes.clear();
        watchLater.clear();
        generosFavoritos.clear();}

    void registrarVista(int movieID) {
        auto it = find(vistasRecientes.begin(), vistasRecientes.end(), movieID);
        if (it != vistasRecientes.end()) vistasRecientes.erase(it);
        vistasRecientes.push_back(movieID);

        if (vistasRecientes.size() > MAX_RECIENTES) { //Mantenemos solo las ultimas mas recientes (20)
            vistasRecientes.erase(vistasRecientes.begin());}}

    const vector<int>& getVistasRecientes() const {return vistasRecientes;}
};


#endif //REPO_PROGRA3_USUARIOHISTORIAL_H