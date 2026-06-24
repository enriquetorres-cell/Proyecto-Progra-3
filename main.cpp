#include <iostream>
#include <vector>
#include <string>
#include <limits>

#include "Catalogo.h"
#include "Movie.h"
#include "dataframe.h"
#include "tokenizador.h"
#include "Usuario.h"
#include "UsuarioHistorial.h"
#include "Sesion.h"
#include "GestionUsuarios.h"
#include "Recomendador.h"
#include "index/Trie.h"
#include "index/Ngramindex.h"
#include "index/Indexador.h"
#include "search/Buscador.h"
#include "search/RankingHibrido.h"

using namespace std;


static Catalogo            g_catalogo;
static Trie                g_trie;
static NgramIndex          g_ngram;
static GestionUsuarios&    g_gestion = GestionUsuarios::getInstance();
static Sesion&             g_sesion  = Sesion::getInstance();
static RankingHibrido*     g_strategy     = nullptr;
static Buscador*           g_buscador     = nullptr;
static Recomendador*       g_recomendador = nullptr;
static vector<string>      g_stopwords;

static const string ARCHIVO_CSV       = "wiki_movie_plots_deduped.csv";
static const string ARCHIVO_STOPWORDS = "palabras_despreciables.txt";
static const int    RESULTADOS_POR_PAGINA = 5;


void home();
void profilePage();
void search();
void movieDetails(int movieId);


static void limpiarBufferEntrada() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

static int leerOpcion() {
    int opcion;
    if (!(cin >> opcion)) {
        limpiarBufferEntrada();
        return -1;
    }
    return opcion;
}


void loadingScreen() {
    cout << "-----------------------------------------------------------------------------------------------------\n"
            "Bienvenido!\n"
            "Cargando catalogo, indices y usuarios...\n";

    dataframe df;
    guardarDF(ARCHIVO_CSV, df);
    for (Movie* m : df) g_catalogo.agregar(m);
    cout << "  - Catalogo cargado (" << g_catalogo.size() << " peliculas)\n";

    g_stopwords = palabrasDespreciables(ARCHIVO_STOPWORDS);
    Indexador::indexar(g_catalogo, g_trie, g_ngram, g_stopwords, true);
    cout << "  - Indices construidos\n";

    g_strategy     = new RankingHibrido(g_catalogo);
    g_buscador     = new Buscador(g_catalogo, g_trie, g_ngram, *g_strategy);
    g_recomendador = new Recomendador(g_catalogo);

    g_gestion.cargarDesdeArchivo();
    cout << "  - Perfiles cargados (" << g_gestion.cantidad() << " usuarios)\n";

    cout << "Carga completa!\n";}


void selectProfile() {
    if (g_gestion.estaVacio()) {
        cout << "No hay perfiles disponibles. Crea uno primero.\n";
        return;}

    cout << "-----------------------------------------------------------------------------------------------------\n"
            "Selecciona un perfil:\n";
    vector<pair<int, string>> perfiles = g_gestion.listar();
    for (size_t i = 0; i < perfiles.size(); i++) {
        cout << "[" << (i + 1) << "] " << perfiles[i].second << "\n";}
    cout << "Opcion: ";

    int opcion = leerOpcion();
    if (opcion < 1 || opcion > (int)perfiles.size()) {
        cout << "Opcion invalida.\n";
        return;}

    int idElegido = perfiles[opcion - 1].first;
    Usuario* u = g_gestion.seleccionar(idElegido);
    if (u == nullptr) {
        cout << "Perfil no encontrado.\n";
        return;}

    if (g_sesion.hayUsuarioActivo()) g_sesion.logout();

    if (!g_sesion.login(u)) {
        cout << "No se pudo iniciar sesion.\n";
        return;}
    cout << "Bienvenido, " << u->getNombre() << "!\n";}

void createProfile() {
    if (g_gestion.estaLleno()) {
        cout << "Ya hay " << GestionUsuarios::MAX_USUARIOS
             << " perfiles. Elimina uno antes de crear otro.\n";
        return;}

    cout << "Nombre del nuevo perfil: ";
    string nombre;
    limpiarBufferEntrada();
    getline(cin, nombre);

    if (nombre.empty()) {
        cout << "Nombre invalido.\n";
        return;}

    Usuario* nuevo = g_gestion.crear(nombre);
    if (nuevo == nullptr) {
        cout << "No se pudo crear el perfil.\n";
        return;}
    cout << "Perfil '" << nombre << "' creado.\n";}

void deleteProfile() {
    if (g_gestion.estaVacio()) {
        cout << "No hay perfiles para eliminar.\n";
        return;}

    cout << "Selecciona un perfil para eliminar:\n";
    vector<pair<int, string>> perfiles = g_gestion.listar();
    for (size_t i = 0; i < perfiles.size(); i++) {
        cout << "[" << (i + 1) << "] " << perfiles[i].second << "\n";}
    cout << "Opcion: ";

    int opcion = leerOpcion();
    if (opcion < 1 || opcion > (int)perfiles.size()) {
        cout << "Opcion invalida.\n";
        return;}

    int idEliminar = perfiles[opcion - 1].first;
    string nombre  = perfiles[opcion - 1].second;

    if (g_gestion.eliminar(idEliminar)) {
        cout << "Perfil '" << nombre << "' eliminado.\n";}
    else {cout << "No se pudo eliminar.\n";}
}

void sessionControl() {
    if (!g_sesion.hayUsuarioActivo()) {
        cout << "No hay sesion activa. Volviendo a la pantalla de perfiles...\n";
        profilePage();}}


void like(int movieId) {
    Usuario* activo = g_sesion.getActivo();
    if (activo == nullptr) return;

    const Movie* peli = g_catalogo.getById(movieId);
    if (peli == nullptr) return;

    activo->darLikeYRegistrarGeneros(movieId, peli->genres);
    g_gestion.guardarEnArchivo();
    cout << "Like registrado para '" << peli->title << "'.\n";}

void watchLater(int movieId) {
    Usuario* activo = g_sesion.getActivo();
    if (activo == nullptr) return;

    const Movie* peli = g_catalogo.getById(movieId);
    if (peli == nullptr) return;

    activo->agregarVerMasTarde(movieId);
    g_gestion.guardarEnArchivo();
    cout << "'" << peli->title << "' agregada a Ver mas tarde.\n";}

void movieDetails(int movieId) {
    const Movie* peli = g_catalogo.getById(movieId);
    if (peli == nullptr) {
        cout << "Pelicula no encontrada.\n";
        home();
        return;}

    cout << "-----------------------------------------------------------------------------------------------------\n"
         << "Titulo:   " << peli->title << " (" << peli->releaseYear << ")\n"
         << "Director: " << peli->director << "\n"
         << "Generos:  ";
    for (size_t i = 0; i < peli->genres.size(); i++) {
        cout << peli->genres[i];
        if (i + 1 < peli->genres.size()) cout << ", ";}
    cout << "\n"
         << "Origen:   " << peli->origin << "\n"
         << "Sinopsis: " << peli->plot << "\n\n"
         << "[1] Like\n"
         << "[2] Ver mas tarde\n"
         << "[3] Volver al menu\n";

    int opcion = leerOpcion();
    if (opcion == 1) {
        like(movieId);
        movieDetails(movieId);}
    else if (opcion == 2) {
        watchLater(movieId);
        movieDetails(movieId);}
    else if (opcion == 3) {
        home();}
    else {
        cout << "Respuesta invalida.\n";
        movieDetails(movieId);}}


void resultDisplay(const vector<int>& ids, int pag) {
    int inicio = pag * RESULTADOS_POR_PAGINA;
    int fin    = min((int)ids.size(), inicio + RESULTADOS_POR_PAGINA);

    for (int i = inicio; i < fin; i++) {
        const Movie* peli = g_catalogo.getById(ids[i]);
        if (peli == nullptr) continue;
        cout << "[" << (i - inicio + 1) << "] "
             << peli->title << " (" << peli->releaseYear << ")\n";}
}

void searchResults(const vector<int>& ids, int pag) {
    if (ids.empty()) {
        cout << "No se encontraron resultados.\n";
        home();
        return;}

    int totalPaginas = ((int)ids.size() + RESULTADOS_POR_PAGINA - 1) / RESULTADOS_POR_PAGINA;
    if (pag < 0) pag = 0;
    if (pag >= totalPaginas) pag = totalPaginas - 1;

    cout << "-----------------------------------------------------------------------------------------------------\n";
    resultDisplay(ids, pag);
    cout << "\nPagina: " << (pag + 1) << " de " << totalPaginas << "\n"
         << "[1-5] Ver detalles\n"
         << "[6] Siguiente pagina\n"
         << "[7] Volver al buscador\n"
         << "[8] Volver al menu\n";

    int opcion = leerOpcion();

    int inicio = pag * RESULTADOS_POR_PAGINA;
    int disponibles = min(RESULTADOS_POR_PAGINA, (int)ids.size() - inicio);

    if (opcion >= 1 && opcion <= 5) {
        if (opcion > disponibles) {
            cout << "Esa opcion no existe en esta pagina.\n";
            searchResults(ids, pag);
            return;}
        movieDetails(ids[inicio + opcion - 1]);}
    else if (opcion == 6) {
        if (pag + 1 >= totalPaginas) {
            cout << "Ya estas en la ultima pagina.\n";
            searchResults(ids, pag);}
        else {searchResults(ids, pag + 1);}}
    else if (opcion == 7) {
        search();}
    else if (opcion == 8) {
        home();}
    else {
        cout << "Respuesta invalida.\n";
        searchResults(ids, pag);}}

void search() {
    cout << "-----------------------------------------------------------------------------------------------------\n"
            "Ingresar consulta (palabra, frase, tag:<genero>, director:<nombre>): ";
    string consulta;
    limpiarBufferEntrada();
    getline(cin, consulta);

    Usuario* activo = g_sesion.getActivo();
    vector<int> resultados = g_buscador->buscar(consulta, activo);
    searchResults(resultados, 0);}

void home() {
    sessionControl();
    Usuario* activo = g_sesion.getActivo();
    if (activo == nullptr) return;

    UsuarioHistorial* hist = activo->getHistorial();

    cout << "-----------------------------------------------------------------------------------------------------\n"
         << "Bienvenido, " << activo->getNombre() << "\n\n";

    cout << "Tus recomendaciones actuales:\n";
    vector<int> recos = g_recomendador->recomendar(*hist, 5);
    if (recos.empty()) {
        cout << "  (Aun no tienes recomendaciones. Dale like a peliculas para personalizar tu Home.)\n";}
    else {
        for (int id : recos) {
            const Movie* peli = g_catalogo.getById(id);
            if (peli != nullptr) cout << "  - " << peli->title << "\n";}}

    cout << "\nLista de Ver mas tarde:\n";
    const vector<int>& watch = hist->getWatchLater();
    if (watch.empty()) {
        cout << "  (vacia)\n";}
    else {
        for (int id : watch) {
            const Movie* peli = g_catalogo.getById(id);
            if (peli != nullptr) cout << "  - " << peli->title << "\n";}}

    cout << "\nTus categorias favoritas:\n";
    const map<string, int>& favoritos = hist->getGenerosFavoritos();
    if (favoritos.empty()) {
        cout << "  (sin categorias aun)\n";}
    else {
        for (const auto& par : favoritos) {
            cout << "  - " << par.first << " (" << par.second << ")\n";}}

    cout << "\n[1] Acceso a busqueda\n"
         << "[2] Logout\n"
         << "[3] Salir\n";

    int opcion = leerOpcion();
    if (opcion == 1) {
        search();}
    else if (opcion == 2) {
        g_gestion.guardarEnArchivo();
        g_sesion.logout();
        profilePage();}
    else if (opcion == 3) {
        g_gestion.guardarEnArchivo();
        exit(0);}
    else {
        cout << "Respuesta invalida.\n";
        home();}}


void profilePage() {
    cout << "-----------------------------------------------------------------------------------------------------\n";
    if (g_gestion.estaVacio()) {
        cout << "(No hay perfiles creados)\n";}
    else if (g_gestion.estaLleno()) {
        cout << "(Limite de " << GestionUsuarios::MAX_USUARIOS << " perfiles alcanzado)\n";}
    cout << "[1] Seleccionar perfil existente\n"
         << "[2] Crear perfil\n"
         << "[3] Eliminar perfil\n"
         << "[4] Salir del programa\n";

    int opcion = leerOpcion();
    if (opcion == 1) {
        selectProfile();
        if (g_sesion.hayUsuarioActivo()) {
            home();}
        else {
            profilePage();}}
    else if (opcion == 2) {
        createProfile();
        profilePage();}
    else if (opcion == 3) {
        deleteProfile();
        profilePage();}
    else if (opcion == 4) {
        g_gestion.guardarEnArchivo();
        exit(0);}
    else {
        cout << "Respuesta invalida.\n";
        profilePage();}}


void runApp() {
    loadingScreen();
    profilePage();}

int main() {
    runApp();

    delete g_buscador;
    delete g_strategy;
    delete g_recomendador;
    return 0;}