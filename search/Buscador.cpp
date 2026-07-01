#include "Buscador.h"
#include "Query.h"

Buscador::Buscador(const Catalogo& cat,
                   const Trie& t,
                   const NgramIndex& n,
                   const RankingStrategy& s)
    : catalogo(cat), trie(t), ngram(n), strategy(s) {}

static string aMinusculas(const string& s) {
    string r;
    r.reserve(s.size());
    for (char c : s) r += (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c;
    return r;
}

static string soloLetras(const string& s) {
    string r;
    for (char c : s) if (c >= 'a' && c <= 'z') r += c;
    return r;
}

vector<int> Buscador::buscar(const string& consulta, const Usuario* usuario) const {
    OrQuery        raiz;
    vector<string> palabras;

    string actual;
    string texto = aMinusculas(consulta);
    texto.push_back(' ');

    for (char c : texto) {
        if (c != ' ' && c != '\t') { actual += c; continue; }
        if (actual.empty()) continue;

        size_t pos = actual.find(':');
        if (pos != string::npos) {
            string prefijo = actual.substr(0, pos);
            string valor   = actual.substr(pos + 1);
            if (!valor.empty()) {
                if (prefijo == "director") raiz.agregar(new DirectorQuery(actual));
                else                       raiz.agregar(new TagQuery(actual));
                string palabra = valor;
                for (char& ch : palabra) if (ch == '_') ch = ' ';
                palabras.push_back(palabra);}}
        else {
            string palabra = soloLetras(actual);
            if (palabra.size() >= 2) {
                raiz.agregar(new TermQuery(palabra));
                palabras.push_back(palabra);
            }}
        actual.clear();}


    set<int> ids = raiz.evaluar(trie, ngram);
    return strategy.rankear(ids, palabras, usuario);}