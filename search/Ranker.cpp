#include "Ranker.h"
#include <algorithm>

Ranker::Ranker(const Catalogo& cat) : catalogo(cat) {}

// Pasa un texto a minusculas para comparar sin importar mayusculas.
static string aMinusculas(const string& s) {
    string r;
    r.reserve(s.size());
    for (char c : s) r += (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c;
    return r;
}

// true si 'aguja' aparece como substring dentro de 'texto'.
static bool contiene(const string& texto, const string& aguja) {
    if (aguja.empty()) return false;
    return texto.find(aguja) != string::npos;
}

vector<int> Ranker::rankear(const set<int>& ids,
                            const vector<string>& palabras,
                            const Usuario* usuario) const {
    // Generos que el usuario ha marcado con like (vacio si no hay sesion).
    const map<string, int>* favoritos = nullptr;
    if (usuario != nullptr) {
        favoritos = &usuario->getHistorial()->getGenerosFavoritos();
    }

    vector<pair<int, double>> conPuntaje;   // (id, score)
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

        // +2 si comparte algun genero con los likes del usuario.
        if (favoritos != nullptr) {
            for (const string& g : peli->genres) {
                if (favoritos->count(g)) { pts += 2; break; }
            }
        }

        // Nota: Ranking.txt multiplica por (pelicula.score / 10). Movie no
        // expone un campo 'score', asi que ese factor se omite aqui.
        conPuntaje.push_back({id, pts});
    }

    // Orden descendente por puntaje (estable: empate -> id menor primero).
    stable_sort(conPuntaje.begin(), conPuntaje.end(),
                [](const pair<int, double>& a, const pair<int, double>& b) {
                    return a.second > b.second;
                });

    vector<int> ordenados;
    ordenados.reserve(conPuntaje.size());
    for (const auto& par : conPuntaje) ordenados.push_back(par.first);
    return ordenados;
}
