#include "Indexador.h"
#include "../tokenizador.h"
#include <algorithm>
#include <thread>
#include <functional>
#include <utility>

using Entrada = pair<string, int>;

// Lowercase + trim. Reemplaza espacios por '_'
static string normalizarTag(const string& s) {
    string r;
    r.reserve(s.size());
    for (char c : s) {
        if (c >= 'A' && c <= 'Z') r += (char)(c + 32);
        else r += c;}
    size_t ini = r.find_first_not_of(" \t");
    size_t fin = r.find_last_not_of(" \t");
    if (ini == string::npos) return "";
    r = r.substr(ini, fin - ini + 1);

    for (char& c : r) if (c == ' ') c = '_';
    return r;}


static void procesarBloque(const vector<Movie*>& peliculas,
                           size_t inicio, size_t fin,
                           const vector<string>& stopwords,
                           bool incluirPlot,
                           vector<Entrada>& trieOut,
                           vector<Entrada>& ngramOut) {
    for (size_t i = inicio; i < fin; i++) {
        Movie* m = peliculas[i];
        if (m == nullptr) continue;

        string texto = m->title;
        if (incluirPlot) {
            texto.push_back(' ');
            texto += m->plot;
        }
        vector<string> tokens = tokenizar(texto, stopwords);

        for (const string& token : tokens) {
            trieOut.push_back(Entrada(token, m->id));
            ngramOut.push_back(Entrada(token, m->id));
        }

        if (!m->director.empty()) {
            string dir = normalizarTag(m->director);
            if (!dir.empty()) {
                trieOut.push_back(Entrada("director:" + dir, m->id));
            }
        }

        for (const string& g : m->genres) {
            if (g.empty()) continue;
            string gen = normalizarTag(g);
            if (!gen.empty()) {
                trieOut.push_back(Entrada("tag:" + gen, m->id));
            }
        }
    }
}

void Indexador::indexar(const Catalogo& catalogo,
                        Trie& trie,
                        NgramIndex& ngram,
                        const vector<string>& stopwords,
                        bool incluirPlot) {
    const auto& peliculas = catalogo.getMovies();
    size_t total = peliculas.size();
    if (total == 0) return;

    unsigned int nHilos = thread::hardware_concurrency();
    if (nHilos == 0) nHilos = 4;
    if ((size_t)nHilos > total) nHilos = (unsigned int)total;


    vector<vector<Entrada>> trieParcial(nHilos);
    vector<vector<Entrada>> ngramParcial(nHilos);
    vector<thread> hilos;
    hilos.reserve(nHilos);

    size_t bloque = (total + nHilos - 1) / nHilos;

    for (unsigned int t = 0; t < nHilos; t++) {
        size_t inicio = t * bloque;
        size_t fin    = min(total, inicio + bloque);
        if (inicio >= fin) continue;

        hilos.push_back(thread(procesarBloque,
                                cref(peliculas), inicio, fin,
                                cref(stopwords), incluirPlot,
                                ref(trieParcial[t]), ref(ngramParcial[t])));}

    for (thread& h : hilos) h.join();

    for (size_t t = 0; t < nHilos; t++) {
        for (size_t i = 0; i < trieParcial[t].size(); i++) {
            trie.insertar(trieParcial[t][i].first, trieParcial[t][i].second);
        }
        for (size_t i = 0; i < ngramParcial[t].size(); i++) {
            ngram.insertarPalabra(ngramParcial[t][i].first, ngramParcial[t][i].second);
        }
    }
}