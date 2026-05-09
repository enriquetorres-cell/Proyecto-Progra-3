
#ifndef AVANCE1_TOKENIZADOR_H
#define AVANCE1_TOKENIZADOR_H

#pragma once
#include <vector>
#include <vector>
#include <string>
using namespace std;

vector<string> palabrasDespreciables(const string& archivo);
vector<string> tokenizar(const string& texto, const vector<string>& pDespreciables);
vector<string> generarNgramas(const string& palabra, int n);
void agregarVocabulario(vector<string>& vocabulario, const vector<string>& tokens);


#endif //AVANCE1_TOKENIZADOR_H