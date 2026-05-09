#include "tokenizador.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

vector<string> palabrasDespreciables(const string& archivo) {
    vector<string> palabras;
    ifstream file(archivo);
    if (!file.is_open()) {
        return palabras;}
    string palabra;
    while (getline(file, palabra)) {
        while (!palabra.empty() && (palabra.back() == '\r' || palabra.back() == ' ')) {
            palabra.pop_back();}
        if (!palabra.empty()){
            palabras.push_back(palabra);}}
    file.close();
    sort(palabras.begin(), palabras.end());
    return palabras;}

vector<string> tokenizar(const string& texto, const vector<string>& pDespreciables) {
    vector<string> tokens;
    string palabra = "";
    for (size_t i = 0; i <= texto.size(); i++) {
        char c = (i < texto.size()) ? texto[i] : ' ';
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            if (c >= 'A' && c <= 'Z') c += 32;
            palabra += c;}
        else {
            if (!palabra.empty()) {
                bool esStopWord = binary_search(pDespreciables.begin(), pDespreciables.end(), palabra);
                if (palabra.size() >= 2 && !esStopWord) {
                    tokens.push_back(palabra);}
                palabra = "";}}}
    return tokens;}

vector<string> generarNgramas(const string& palabra, int n) {
    vector<string> ngramas;
    //si la palabra es menor a n entonces no generamos nada
    if ((int)palabra.size() < n) {
        return ngramas;}

    for (size_t i = 0; i <= palabra.size() - n; i++) {
        ngramas.push_back(palabra.substr(i, n));}
    return ngramas;}

void agregarVocabulario(vector<string>& vocabulario, const vector<string>& tokens) {
    for (const string& token : tokens)
        vocabulario.push_back(token);}