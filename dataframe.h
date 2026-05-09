

#ifndef AVANCE1_DATAFRAME_H
#define AVANCE1_DATAFRAME_H
#pragma once
#include "Movie.h"

string limpiarSobrantes(const string& s);
string to_lower(string s);
string normalizarSeparadores(const string& s) ;
bool esDesconocido(const string& s);
string limpiarTexto(const string& s);
vector<string> convertirVector(const string& s, char delimiter);
vector<string> mapeoLinea(const string& linea);
void limpiarMovie(Movie* m);
void guardarDF(const string& nombreArchivo, dataframe& df);
void liberarDF(dataframe& df);

#endif //AVANCE1_DATAFRAME_H