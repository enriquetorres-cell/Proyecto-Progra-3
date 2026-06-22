#include "Buscador.h"
#include "Query.h"

Buscador::Buscador(const Catalogo& cat, const Trie& t, const NgramIndex& n)
    : catalogo(cat), trie(t), ngram(n), ranker(cat) {}

// Pasa a minusculas (los indices guardan todo en minusculas).
static string aMinusculas(const string& s) {
    string r;
    r.reserve(s.size());
    for (char c : s) r += (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c;
    return r;
}

// Deja solo letras a-z. Mismo criterio que tokenizar() del indice,
// asi la palabra de la consulta coincide con como fue indexada.
static string soloLetras(const string& s) {
    string r;
    for (char c : s) if (c >= 'a' && c <= 'z') r += c;
    return r;
}

vector<int> Buscador::buscar(const string& consulta, const Usuario* usuario) const {
    OrQuery       raiz;        // raiz del arbol Composite
    vector<string> palabras;   // terminos normalizados (para el ranking)

    // Separa la consulta en palabras por espacios.
    string actual;
    string texto = aMinusculas(consulta);
    texto.push_back(' ');   // centinela para cerrar la ultima palabra

    for (char c : texto) {
        if (c != ' ' && c != '\t') { actual += c; continue; }
        if (actual.empty()) continue;

        // Caso 1: tag especial -> contiene ':' ("tag:horror", "director:nolan").
        size_t pos = actual.find(':');
        if (pos != string::npos) {
            string prefijo = actual.substr(0, pos);
            string valor   = actual.substr(pos + 1);
            if (!valor.empty()) {
                if (prefijo == "director") raiz.agregar(new DirectorQuery(actual));
                else                       raiz.agregar(new TagQuery(actual));
                // Para el ranking: el valor sin prefijo, '_' -> espacio.
                string palabra = valor;
                for (char& ch : palabra) if (ch == '_') ch = ' ';
                palabras.push_back(palabra);
            }
        }
        // Casos 2 y 3: palabra normal de 2+ letras -> hoja TermQuery.
        else {
            string palabra = soloLetras(actual);
            if (palabra.size() >= 2) {
                raiz.agregar(new TermQuery(palabra));
                palabras.push_back(palabra);
            }
            // 1 letra o menos: se ignora (Busqueda.txt, caso 4).
        }
        actual.clear();
    }

    // Evalua el arbol polimorficamente y rankea el resultado.
    set<int> ids = raiz.evaluar(trie, ngram);
    return ranker.rankear(ids, palabras, usuario);
}
