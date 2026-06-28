#include "GestionUsuarios.h"
#include "Sesion.h"
#include "UsuarioHistorial.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Singleton boilerplate

GestionUsuarios::GestionUsuarios() : proximoId(0) {}

GestionUsuarios& GestionUsuarios::getInstance() {
    static GestionUsuarios instance;
    return instance;
}

GestionUsuarios::~GestionUsuarios() {
    // Por seguridad: si Sesion todavia tiene un activo apuntando a uno
    // de nuestros usuarios, hacemos logout antes de borrarlos.
    Sesion::getInstance().logout();
    for (Usuario* u : usuarios) {
        delete u;
    }
    usuarios.clear();
}

// Gestion de perfiles

Usuario* GestionUsuarios::crear(const std::string& nombre) {
    if (usuarios.size() >= MAX_USUARIOS) return nullptr;

    Usuario* u = new Usuario(proximoId, nombre);
    usuarios.push_back(u);
    proximoId++;

    guardarEnArchivo(); // persistir al modificar
    return u;
}

bool GestionUsuarios::eliminar(int id) {
    for (auto it = usuarios.begin(); it != usuarios.end(); ++it) {
        if ((*it)->getId() == id) {
            // Si era el activo en Sesion, hacer logout primero
            if (Sesion::getInstance().getActivo() == *it) {
                Sesion::getInstance().logout();}
            delete *it;
            usuarios.erase(it);
            guardarEnArchivo();
            return true;}}
    return false;}

Usuario* GestionUsuarios::seleccionar(int id) {
    for (Usuario* u : usuarios) {
        if (u->getId() == id) return u;
    }
    return nullptr;
}

std::vector<std::pair<int, std::string>> GestionUsuarios::listar() const {
    std::vector<std::pair<int, std::string>> resultado;
    resultado.reserve(usuarios.size());
    for (const Usuario* u : usuarios) {
        resultado.push_back(std::make_pair(u->getId(), u->getNombre()));
    }
    return resultado;
}

bool GestionUsuarios::estaLleno() const  { return usuarios.size() >= MAX_USUARIOS; }
bool GestionUsuarios::estaVacio() const  { return usuarios.empty(); }
size_t GestionUsuarios::cantidad() const { return usuarios.size(); }

void GestionUsuarios::limpiarTodo() {
    Sesion::getInstance().logout(); // no dejar dangling pointer en Sesion
    for (Usuario* u : usuarios) delete u;
    usuarios.clear();
    proximoId = 0;
}

// Persistencia
//
// Formato (texto plano, una "key" por linea):
//
//   # Perfiles UTEC Progra3 v1
//   USUARIOS <N>
//   PROXIMO_ID <M>
//   ---
//   ID <id>
//   NOMBRE <nombre con espacios>
//   LIKES <k>
//   <movieId_1>
//   <movieId_2>
//   ...
//   WATCHLATER <k>
//   <movieId_1>
//   ...
//   GENEROS <k>
//   <count> <genero con espacios>
//   ...
//   ---
//   (repite por usuario)
//
// Por que asi y no JSON/CSV:
//   - No requiere libreria externa.
//   - Cada lista anuncia su tamanio (LIKES <k>) -> el parser hace un for
//     fijo en vez de adivinar delimitadores. Robusto y predecible.
//   - Los generos pueden tener espacios ("romantic comedy"). Poner el
//     count primero y dejar el resto de la linea como nombre es trivial
//     con istringstream + getline.

// ---- helpers internos (no expuestos) ----

namespace {

// Lee una linea con formato "LABEL <int>" y deja el int en 'valor'.
// Devuelve true si la linea existe y el label coincide.
bool leerLabelInt(std::ifstream& f, const std::string& esperado, int& valor) {
    std::string linea;
    if (!std::getline(f, linea)) return false;
    std::istringstream iss(linea);
    std::string label;
    if (!(iss >> label >> valor)) return false;
    return label == esperado;
}

// Lee una linea con formato "LABEL <int>" y lo guarda en size_t. Igual que arriba
// pero para tamanios. Usamos int internamente porque el archivo siempre va a
// caber en int (max 4 usuarios, listas chicas).
bool leerLabelInt_st(std::ifstream& f, const std::string& esperado, int& valor) {
    return leerLabelInt(f, esperado, valor);
}

// Lee una linea con formato "NOMBRE <texto que puede tener espacios>".
// Devuelve el texto en 'valor' (sin el prefijo "NOMBRE ").
bool leerLabelString(std::ifstream& f, const std::string& esperado, std::string& valor) {
    std::string linea;
    if (!std::getline(f, linea)) return false;
    // Buscar el primer espacio para separar label del resto
    size_t sp = linea.find(' ');
    if (sp == std::string::npos) return false;
    std::string label = linea.substr(0, sp);
    if (label != esperado) return false;
    valor = linea.substr(sp + 1);
    return true;
}

// Lee una linea con un solo entero.
bool leerInt(std::ifstream& f, int& valor) {
    std::string linea;
    if (!std::getline(f, linea)) return false;
    std::istringstream iss(linea);
    return static_cast<bool>(iss >> valor);
}

// Lee una linea con formato "<count> <texto con espacios>".
bool leerCountString(std::ifstream& f, int& count, std::string& texto) {
    std::string linea;
    if (!std::getline(f, linea)) return false;
    size_t sp = linea.find(' ');
    if (sp == std::string::npos) return false;
    std::istringstream iss(linea.substr(0, sp));
    if (!(iss >> count)) return false;
    texto = linea.substr(sp + 1);
    return true;
}

} // namespace anonimo

// ---- guardar ----

bool GestionUsuarios::guardarEnArchivo(const std::string& ruta) const {
    std::ofstream f(ruta);
    if (!f.is_open()) return false;

    f << "# Perfiles UTEC Progra3 v1\n";
    f << "USUARIOS "   << usuarios.size() << "\n";
    f << "PROXIMO_ID " << proximoId       << "\n";

    for (const Usuario* u : usuarios) {
        f << "---\n";
        f << "ID "     << u->getId()     << "\n";
        f << "NOMBRE " << u->getNombre() << "\n";

        const UsuarioHistorial* h = u->getHistorial();

        const std::set<int>& likes = h->getLikes();
        f << "LIKES " << likes.size() << "\n";
        for (int mid : likes) f << mid << "\n";

        const std::vector<int>& wl = h->getWatchLater();
        f << "WATCHLATER " << wl.size() << "\n";
        for (int mid : wl) f << mid << "\n";

        const std::vector<int>& vistas = h->getVistasRecientes();
        f << "VISTAS " << vistas.size() << "\n";
        for (int mid : vistas) f << mid << "\n";

        const std::map<std::string, int>& gens = h->getGenerosFavoritos();
        f << "GENEROS " << gens.size() << "\n";
        for (const auto& par : gens) {
            f << par.second << " " << par.first << "\n";}}

    return f.good();
}

// ---- cargar ----
//
// Estrategia "todo o nada": construye un vector temporal 'nuevos' y solo
// reemplaza el estado interno si TODO el parse fue exitoso. Si algo falla,
// libera 'nuevos' y devuelve false sin tocar el estado actual.

bool GestionUsuarios::cargarDesdeArchivo(const std::string& ruta) {
    std::ifstream f(ruta);
    if (!f.is_open()) return false; // archivo no existe -> primer arranque

    std::vector<Usuario*> nuevos;
    int nuevoProximoId = 0;
    bool ok = true;

    auto liberarYFallar = [&]() {
        for (Usuario* u : nuevos) delete u;
        nuevos.clear();
        return false;
    };

    std::string linea;

    // Saltar comentarios iniciales (lineas que empiezan con '#') y vacias
    while (std::getline(f, linea)) {
        if (linea.empty()) continue;
        if (linea[0] == '#') continue;
        break;
    }
    if (linea.empty()) return liberarYFallar();

    // linea debe ser "USUARIOS <N>"
    int numUsuarios = 0;
    {
        std::istringstream iss(linea);
        std::string label;
        if (!(iss >> label >> numUsuarios)) return liberarYFallar();
        if (label != "USUARIOS") return liberarYFallar();
    }

    if (!leerLabelInt(f, "PROXIMO_ID", nuevoProximoId)) return liberarYFallar();

    for (int i = 0; i < numUsuarios && ok; i++) {
        // Separador "---"
        if (!std::getline(f, linea) || linea != "---") { ok = false; break; }

        // ID
        int id = 0;
        if (!leerLabelInt(f, "ID", id)) { ok = false; break; }

        // NOMBRE
        std::string nombre;
        if (!leerLabelString(f, "NOMBRE", nombre)) { ok = false; break; }

        Usuario* u = new Usuario(id, nombre);
        UsuarioHistorial* h = u->getHistorial();

        // LIKES
        int n = 0;
        if (!leerLabelInt(f, "LIKES", n)) { delete u; ok = false; break; }
        bool sub_ok = true;
        for (int j = 0; j < n; j++) {
            int mid;
            if (!leerInt(f, mid)) { sub_ok = false; break; }
            h->agregarLike(mid);
        }
        if (!sub_ok) { delete u; ok = false; break; }

        // WATCHLATER
        if (!leerLabelInt(f, "WATCHLATER", n)) { delete u; ok = false; break; }
        for (int j = 0; j < n; j++) {
            int mid;
            if (!leerInt(f, mid)) { sub_ok = false; break; }
            h->agregarVerMasTarde(mid);
        }
        if (!sub_ok) { delete u; ok = false; break; }

        // VISTAS
        if (!leerLabelInt(f, "VISTAS", n)) { delete u; ok = false; break; }
        for (int j = 0; j < n; j++) {
            int mid;
            if (!leerInt(f, mid)) { sub_ok = false; break; }
            h->registrarVista(mid);}
        if (!sub_ok) { delete u; ok = false; break; }

        // GENEROS
        if (!leerLabelInt(f, "GENEROS", n)) { delete u; ok = false; break; }
        for (int j = 0; j < n; j++) {
            int count = 0;
            std::string genero;
            if (!leerCountString(f, count, genero)) { sub_ok = false; break; }
            // registrarGenero suma 1 por llamada. Para reconstruir el conteo
            // original llamamos 'count' veces. Los counts son chicos (cuantos
            // generos puede consumir un usuario? pocas decenas).
            for (int k = 0; k < count; k++) h->registrarGenero(genero);
        }
        if (!sub_ok) { delete u; ok = false; break; }

        nuevos.push_back(u);
    }

    if (!ok) return liberarYFallar();

    // Exito: reemplazar estado actual
    Sesion::getInstance().logout(); // invalidar sesion previa
    for (Usuario* u : usuarios) delete u;
    usuarios = std::move(nuevos);
    proximoId = nuevoProximoId;
    return true;
}