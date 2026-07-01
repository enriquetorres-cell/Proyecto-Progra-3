#include "RankingHibrido.h"
#include "RankingUtils.h"
#include <algorithm>

RankingHibrido::RankingHibrido(const Catalogo& cat) : RankingStrategy(cat) {}

static string aMinusculas(const string& s) {
    string r;
    r.reserve(s.size());
    for (char c : s) r += (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c;
    return r;
}

static bool contiene(const string& texto, const string& aguja) {
    if (aguja.empty()) return false;
    return texto.find(aguja) != string::npos;
}

vector<int> RankingHibrido::rankear(const set<int>& ids,
                                    const vector<string>& palabras,
                                    const Usuario* usuario) const {
    const map<string, int>* favoritos = nullptr;
    if (usuario != nullptr) {
        favoritos = &usuario->getHistorial()->getGenerosFavoritos();
    }

    vector<pair<int, double>> conPuntaje;
    conPuntaje.reserve(ids.size());

    for (int id : ids) {
        const Movie* peli = catalogo.getById(id);
        if (peli == nullptr) continue;

        string titulo   = aMinusculas(peli->title);
        string director = aMinusculas(peli->director);
        string sinopsis = aMinusculas(peli->plot);

        double pts = 0.0;
        for (const string& palabra : palabras) {
            if (contiene(titulo,   palabra)) pts += 3;
            if (contiene(director, palabra)) pts += 2;
            if (contiene(sinopsis, palabra)) pts += 1;
        }
        if (favoritos != nullptr) {
            for (const string& g : peli->genres) {
                if (favoritos->count(g)) { pts += 2; break; }
            }
        }

        conPuntaje.push_back({id, pts});
    }

    return ordenarPorPuntaje(move(conPuntaje),
        [](const pair<int, double>& a, const pair<int, double>& b) {
            return a.second > b.second;
        });}