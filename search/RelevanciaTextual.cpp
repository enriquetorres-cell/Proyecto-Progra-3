#include "RelevanciaTextual.h"
#include "RankingUtils.h"
#include <algorithm>

RelevanciaTextual::RelevanciaTextual(const Catalogo& cat) : RankingStrategy(cat) {}

static string aMinusculasRT(const string& s) {
    string r;
    r.reserve(s.size());
    for (char c : s) r += (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c;
    return r;}


static bool contienePalabraCompleta(const string& texto, const string& palabra) {
    if (palabra.empty()) return false;
    size_t pos = 0;
    while ((pos = texto.find(palabra, pos)) != string::npos) {
        bool inicioOk = (pos == 0) ||
                        !((texto[pos-1] >= 'a' && texto[pos-1] <= 'z'));
        size_t fin = pos + palabra.size();
        bool finOk = (fin == texto.size()) ||
                     !((texto[fin] >= 'a' && texto[fin] <= 'z'));
        if (inicioOk && finOk) return true;
        pos += 1;}
    return false;}

vector<int> RelevanciaTextual::rankear(const set<int>& ids, const vector<string>& palabras, const Usuario* usuario) const {
    (void)usuario;

    vector<pair<int, double>> conPuntaje;
    conPuntaje.reserve(ids.size());

    for (int id : ids) {
        const Movie* peli = catalogo.getById(id);
        if (peli == nullptr) continue;

        string titulo   = aMinusculasRT(peli->title);
        string director = aMinusculasRT(peli->director);
        string sinopsis = aMinusculasRT(peli->plot);

        double pts = 0.0;
        for (const string& palabra : palabras) {
            if (contienePalabraCompleta(titulo,   palabra)) pts += 3;
            if (contienePalabraCompleta(director, palabra)) pts += 2;
            if (contienePalabraCompleta(sinopsis, palabra)) pts += 1;
        }

        if (pts > 0) conPuntaje.push_back({id, pts});}

    return ordenarPorPuntaje(move(conPuntaje),
        [this](const pair<int, double>& a, const pair<int, double>& b) {
            if (a.second != b.second) return a.second > b.second;
            const Movie* ma = catalogo.getById(a.first);
            const Movie* mb = catalogo.getById(b.first);
            int yearA = (ma ? ma->releaseYear : 0);
            int yearB = (mb ? mb->releaseYear : 0);
            return yearA > yearB;});}