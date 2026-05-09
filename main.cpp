#include <iostream>
#include <list>
using namespace std;
void home();
void profilePage();
void search();
void movieDetails(string& peli);
void loadingScreen() {
    cout << "-----------------------------------------------------------------------------------------------------\n"
            "Bienvenido!\n"
    // Carga de catalogo, indices y usuarios
            "Carga de catalogo, indices y usuarios completa!\n";
    // Avisar si hubo errores al cargar archivos
}
void selectProfile() {
    // El sistema selecciona al perfil activo
}
void createProfile() {
    //Se crea un nuevo perfil
}
void deleteProfile() {
    //Se crea un nuevo perfil
}
void sessionControl() {
    // Funcion que se asegura de que hay un usuario activo
}

void like(string& peli) {
    //funcion para darle like a la peli;
    cout << "like registrado\n";
    movieDetails(peli);
}
void watchLater(string& peli) {
    //funcion para poner la peli en ver mas tarde;
    cout << "Pelicula anhadida a ver mas tarde\n";
    movieDetails(peli);
}
void movieDetails(string& peli) {
    int opcion;
    cout << "-----------------------------------------------------------------------------------------------------\n"
            "Titulo de la pelicula: " << peli << "\n" // Nombre de la pela
            "Sinopsis: " << /*sinopsis de la peli*/ "\n"
            "Tags: "<< /*tags de la peli*/ "\n"
            "Genero: "<< /*genero de la peli*/ "\n"
            "director: "<< /*director de la peli*/ "\n"
            "[1] Like\n"
            "[2] Ver mas tarde\n"
            "[3] Volver al menu\n";
    cin >> opcion;
    if (opcion == 1) {
        like(peli);
    } else if (opcion == 2) {
        watchLater(peli);
    }
    else if (opcion == 3) {
        home();
    } else {
        cout << "Respuesta invalida\n";
        movieDetails(peli);
    }
}
void resultDisplay(list<string>& pelis, int pag) {
    //Funcion que imprimira los 5 resultados
}
void searchResults(list<string>& pelis, int pag) {
    int opcion;
    resultDisplay(pelis, pag);
    cout << "Pagina: 1 de [Numero de paginas de la consulta]\n" // Funcion que devuelva el numero de paginas de la
                                                              // consulta
            "[1] Primera opcion\n"
            "[2] Segunda opcion\n"
            "[3] Tercea opcion\n"
            "[4] Cuarta opcion\n"
            "[5] Quinta opcion\n" //Eventualmente, esto se reemplazara esto con:
                                  //"[x] opcion numero [ranking de importancia del resultado]"
            "[6] Siguiente pagina\n"
            "[7] volver al buscador\n"
            "[8] volver al menu\n";
    cin >> opcion;
    // Se usa la variable para hallar la peli objetivo del resultado de la consulta
    // Tambien se actualiza las preferencias del cliente en su perfil
    string pelicula = "Batman"; //variable temporal para reemplazar el resultado final dela consulta
    if (opcion == 1 or opcion == 2 or opcion == 3 or opcion == 4 or opcion == 5) {
        movieDetails(pelicula);
    } else if (opcion == 6) {
        pag++;
        searchResults(pelis, pag);
    } else if (opcion == 7) {
        search();
    } else if (opcion == 8) {
        home();
    } else {
        cout << "Respuesta invalida\n";
        searchResults(pelis, pag);
    }
}
void search() {
    string consulta;
    cout << "-----------------------------------------------------------------------------------------------------\n"
            "Ingresar consulta: "; //Soportar busqueda completa, busqueda por tag y por fragmento
    getline(cin >> ws, consulta);
    // Funcion(es) que hace(n) la consulta en si devuelve el resultado
    list<string> resultados; // variable que reemplaza los resultados
    searchResults(resultados, 0);
}
void home() {
    sessionControl();
    int opcion = 0;
    cout << "-----------------------------------------------------------------------------------------------------\n"
            "Bienvenido, [nombre del usuario]\n" // funcion que llama al nombre del usuario
            "Tus recomendaciones actuales: [lista de recomendaciones]\n" // funcion que llama a la lista de
                                                                         // recomendaciones del usuario
            "Lista de ver mas tarde: [lista de ver mas tarde]\n" // funcion que llama a la lista de ver mas tarde
                                                                 // el usuario
            "Tus categorias favoritas: [lista de tags]\n" // Funcion que llama a la lista de categorias favoritas
                                                           // del usuario
            "[1] Acceso a busqueda\n"
            "[2] Logout\n"
            "[3] Salir\n";
    cin >> opcion;
    if (opcion == 1) {
        search();
    } else if (opcion == 2) {
        profilePage();
    } else if (opcion == 3) {
        exit(0);
    } else {
        cout << "Respuesta invalida\n";
        home();
    }
}
void profilePage() {
    int opcion = 0;
    cout << "-----------------------------------------------------------------------------------------------------\n"
            "[1] Seleccionar perfil existente\n" //Indicar si no hay perfiles
            "[2] Crear perfil\n"                 //Indicar si hay 4 perfiles
            "[3] Eliminar perfil\n"              // Indicar si no hay perfiles
            "[4] Salir del programa\n";
    cin >> opcion;
    if (opcion == 1) {
        selectProfile();
        home();
    } else if (opcion == 2) {
        createProfile();
        profilePage();
    } else if (opcion == 3) {
        deleteProfile();
        profilePage();
    } else if (opcion == 4) {
        exit(0);
    } else {
        cout << "Respuesta invalida\n";
        profilePage();
    }
}
void runApp() {
    loadingScreen();
    profilePage();
}
int main() {
    runApp();
    return 0;
}