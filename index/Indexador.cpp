#include "Indexador.h"
#include "../tokenizador.h"
#include <algorithm>
 
// Lowercase + trim. Reemplaza espacios por '_' para que
// "Christopher Nolan" se inserte como "director:christopher_nolan"
// (una sola key en el trie, busqueda exacta directa).
static string normalizarTag(const string& s) {
    string r;
    r.reserve(s.size());
    for (char c : s) {
        if (c >= 'A' && c <= 'Z') r += (char)(c + 32);
        else r += c;
    }
    size_t ini = r.find_first_not_of(" \t");
    size_t fin = r.find_last_not_of(" \t");
    if (ini == string::npos) return "";
    r = r.substr(ini, fin - ini + 1);
 
    for (char& c : r) if (c == ' ') c = '_';
    return r;
}
 
void Indexador::indexar(const Catalogo& catalogo,
                        Trie& trie,
                        NgramIndex& ngram,
                        const vector<string>& stopwords,
                        bool incluirPlot) {
    const auto& peliculas = catalogo.getMovies();
 
    for (Movie* m : peliculas) {
        if (m == nullptr) continue;
 
        // -----------------------------------------------------
        // 1) Tokens del titulo (+ plot opcionalmente)
        // -----------------------------------------------------
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
 
        // -----------------------------------------------------
        // 2) Tag especial: director:<nombre>
        // -----------------------------------------------------
        if (!m->director.empty()) {
            string dir = normalizarTag(m->director);
            if (!dir.empty()) {
                trie.insertar("director:" + dir, m->id);
            }
        }
 
        // -----------------------------------------------------
        // 3) Tag especial: tag:<genero>
        // -----------------------------------------------------
        for (const string& g : m->genres) {
            if (g.empty()) continue;
            string gen = normalizarTag(g);
            if (!gen.empty()) {
                trie.insertar("tag:" + gen, m->id);
            }
        }
    }
}
 