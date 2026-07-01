#include "Indexador.h"
#include "../tokenizador.h"
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

    for (Movie* m : peliculas) {
        if (m == nullptr) continue;

        string texto = m->title;
        if (incluirPlot) {
            texto.push_back(' ');
            texto += m->plot;
        }
        vector<string> tokens = tokenizar(texto, stopwords);

        for (const string& token : tokens) {
            trie.insertar(token, m->id);
            ngram.insertarPalabra(token, m->id);
        }


        if (!m->director.empty()) {
            string dir = normalizarTag(m->director);
            if (!dir.empty()) {
                trie.insertar("director:" + dir, m->id);
            }
        }


        for (const string& g : m->genres) {
            if (g.empty()) continue;
            string gen = normalizarTag(g);
            if (!gen.empty()) {
                trie.insertar("tag:" + gen, m->id);
            }
        }
    }
}
