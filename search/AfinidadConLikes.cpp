#include "AfinidadConLikes.h"
#include <algorithm>

AfinidadConLikes::AfinidadConLikes(const Catalogo& cat) : RankingStrategy(cat) {}

static string aMinusculasAL(const string& s) {
    string r;
    r.reserve(s.size());
    for (char c : s) r += (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c;
    return r;}

static bool contieneAL(const string& texto, const string& aguja) {
    if (aguja.empty()) return false;
    return texto.find(aguja) != string::npos;}

vector<int> AfinidadConLikes::rankear(const set<int>& ids,
                                      const vector<string>& palabras,
                                      const Usuario* usuario) const {
    const map<string, int>* favoritos = nullptr;
    if (usuario != nullptr) {
        favoritos = &usuario->getHistorial()->getGenerosFavoritos();}

    vector<pair<int, double>> conPuntaje;
    conPuntaje.reserve(ids.size());

    for (int id : ids) {
        const Movie* peli = catalogo.getById(id);
        if (peli == nullptr) continue;

        string titulo   = aMinusculasAL(peli->title);
        string sinopsis = aMinusculasAL(peli->plot);

        double pts = 0.0;

        for (const string& palabra : palabras) {
            if (contieneAL(titulo,   palabra)) pts += 1;
            if (contieneAL(sinopsis, palabra)) pts += 1;
        }

        if (favoritos != nullptr && !favoritos->empty()) {
            for (const string& g : peli->genres) {
                auto it = favoritos->find(g);
                if (it != favoritos->end()) {
                    pts += 3.0 * it->second;}}}
        if (pts > 0) conPuntaje.push_back({id, pts});}

    stable_sort(conPuntaje.begin(), conPuntaje.end(),
                [](const pair<int, double>& a, const pair<int, double>& b) {
                    return a.second > b.second;});

    vector<int> ordenados;
    ordenados.reserve(conPuntaje.size());
    for (const auto& par : conPuntaje) ordenados.push_back(par.first);
    return ordenados;}