#include "dataframe.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>
#include <tuple>
#include <iostream>
#include <thread>


// elimina espacios sobrantes, saltos de linea o tabulaciones a los extremos
string limpiarSobrantes(const string& s) {
    size_t ini = s.find_first_not_of(" \t\r\n");
    size_t fin = s.find_last_not_of(" \t\r\n");
    if (ini == string::npos) return "";
    return s.substr(ini, fin - ini + 1);}

// Convierte string a minúsculas
string to_lower(string s) {
    for (char &c : s) {
        if (c >= 'A' && c <= 'Z') {
            c += 32;}}
    return s;}

// reemplaza separadores / & " and " por ", "
string normalizarSeparadores(const string& s) {
    string res = "";
    size_t i = 0;
    while (i < s.size()) {
        if (i + 5 <= s.size() && to_lower(s.substr(i, 5)) == " and ") {
            res += ", ";
            i += 5;}
        else if (s[i] == '/') {
            res += ", ";
            i++;}
        else if (s[i] == '&') {
            res += ", ";
            i++;}
        else {
            res += s[i];
            i++;}}
    string final = "";
    bool espacioAnterior = false;
    for (char c : res) {
        if (c == ' ') {
            if (!espacioAnterior) final += c;
            espacioAnterior = true;}
        else {
            final += c;
            espacioAnterior = false;}}
    return limpiarSobrantes(final);}

// Verifica si un valor de la columna es desconocido
bool esDesconocido(const string& s) {
    string lower = to_lower(limpiarSobrantes(s));
    return lower == "unknown" || lower == "unknok" || lower == "desconocido" || lower.empty();}


// quita comillas externas y trata las dobles comillas
string limpiarTexto(const string& s) {
    size_t ini = s.find_first_not_of(" \t\r\n\"");
    size_t fin = s.find_last_not_of(" \t\r\n\"");
    if (ini == string::npos) return "";
    string limpio = s.substr(ini, fin - ini + 1);
    string resultado = "";
    for (size_t i = 0; i < limpio.size(); i++) {
        if (limpio[i] == '"' && i + 1 < limpio.size() && limpio[i+1] == '"') {
            resultado += '"'; i++;}
        else {resultado += limpio[i];}}
    return resultado;}

// valores en una lista
vector<string> convertirVector(const string& s, char delimiter) {
    vector<string> valores;
    string valor;
    istringstream tokenStream(s);
    while (getline(tokenStream, valor, delimiter)) {
        valor = limpiarSobrantes(limpiarTexto(valor));
        if (!valor.empty()) valores.push_back(valor);}
    return valores;}

// identificar valores de una columna
vector<string> mapeoLinea(const string& linea) {
    vector<string> columnas;
    string valorActual = "";
    bool dentroComillas = false;
    for (size_t i = 0; i < linea.size(); i++) {
        char c = linea[i];
        if (c == '"') {
            if (dentroComillas && i + 1 < linea.size() && linea[i+1] == '"') {
                valorActual += '"';
                i++;}
            else {
                dentroComillas = !dentroComillas;}}
        else if (c == ',' && !dentroComillas) {
            columnas.push_back(valorActual);
            valorActual = "";}
        else {valorActual += c;}}
    columnas.push_back(valorActual);
    return columnas;}

// aplicamos limpieza
void limpiarMovie(Movie* m) {
    m->title  =limpiarSobrantes(m->title);
    m->plot= limpiarSobrantes(m->plot);
    m->director = limpiarSobrantes(m->director);
    m->origin = to_lower(limpiarSobrantes(m->origin));
    if (esDesconocido(m->director))
        m->director = "";
    else
        m->director = normalizarSeparadores(m->director);

    for (auto& g : m->genres) {
        g = to_lower(limpiarSobrantes(g));
        if (esDesconocido(g)) g = "";
        else g = to_lower(normalizarSeparadores(g));}
    //eliminamos generos vacios del vector
    m->genres.erase(remove_if(m->genres.begin(), m->genres.end(), [](const string& g){ return g.empty(); }),m->genres.end());

    for (auto& actor : m->cast) {
        actor = limpiarSobrantes(actor);
        if (esDesconocido(actor))
            actor = "";
        else
            actor = normalizarSeparadores(actor);}
    //eliminamos actores vacios del vector
    m->cast.erase(
        remove_if(m->cast.begin(), m->cast.end(), [](const string& a){ return a.empty(); }),
        m->cast.end());}

static void procesarBloqueCSV(const vector<string>& lineas, size_t inicio, size_t fin,
                              vector<Movie*>& salida) {
    for (size_t i = inicio; i < fin; i++) {
        const string& linea = lineas[i];
        if (linea.empty()) continue;

        vector<string> columnas = mapeoLinea(linea);
        if (columnas.size() < 8) continue;

        Movie* m = new Movie();
        try { m->releaseYear = stoi(limpiarSobrantes(columnas[0])); }
        catch(...) { m->releaseYear = 0; }

        m->title    = limpiarTexto(columnas[1]);
        m->origin   = limpiarTexto(columnas[2]);
        m->director = limpiarTexto(columnas[3]);
        m->cast     = convertirVector(limpiarTexto(columnas[4]), ',');
        m->genres   = convertirVector(limpiarTexto(columnas[5]), ',');
        m->plot     = limpiarTexto(columnas[7]);
        limpiarMovie(m);

        if (m->releaseYear == 0) { delete m; continue; }
        salida.push_back(m);}}

// guardar el df
void guardarDF(const string& nombreArchivo, dataframe& df) {
    ifstream file(nombreArchivo);
    if (!file.is_open()) {
        cout << "Error: No se encontro el archivo " << nombreArchivo << endl;
        return;}
    string linea;
    getline(file, linea); // saltar cabecera

    vector<string> lineas;
    while (getline(file, linea)) lineas.push_back(linea);
    file.close();

    size_t total = lineas.size();
    if (total == 0) {
        cout << "Dataframe guardado con informacion de: 0 peliculas" << endl;
        return;}

    unsigned int nHilos = thread::hardware_concurrency();
    if (nHilos == 0) nHilos = 4;
    if ((size_t)nHilos > total) nHilos = (unsigned int)total;

    vector<vector<Movie*>> parcial(nHilos);
    vector<thread> hilos;
    hilos.reserve(nHilos);

    size_t bloque = (total + nHilos - 1) / nHilos;
    for (unsigned int t = 0; t < nHilos; t++) {
        size_t inicio = t * bloque;
        size_t fin    = min(total, inicio + bloque);
        if (inicio >= fin) continue;
        hilos.push_back(thread(procesarBloqueCSV, cref(lineas), inicio, fin, ref(parcial[t])));}
    for (thread& h : hilos) h.join();

    set<tuple<string,int,string>> vistos;
    int contadorID = 0;

    for (unsigned int t = 0; t < nHilos; t++) {
        for (Movie* m : parcial[t]) {
            auto key = make_tuple(to_lower(m->title), m->releaseYear, to_lower(m->origin));
            if (vistos.count(key)) { delete m; continue; }
            vistos.insert(key);
            m->id = contadorID++;
            df.push_back(m);}}

    cout << "Dataframe guardado con informacion de: " << df.size() << " peliculas" << endl;}

void liberarDF(dataframe& df) {
    for (auto* m : df) delete m;
    df.clear();}