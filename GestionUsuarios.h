#ifndef REPO_PROGRA3_GESTIONUSUARIOS_H
#define REPO_PROGRA3_GESTIONUSUARIOS_H
#include "Usuario.h"
#include <vector>
#include <string>
#include <utility>

/*
 * GestionUsuarios (Paquete 2)
 *
 * Singleton dueno de un vector<Usuario*> con maximo MAX_USUARIOS slots.
 * Mismo patron que Sesion: una sola instancia en toda la app.
 *
 * Ownership:
 *   - GestionUsuarios es dueno de los Usuario*. Su destructor los libera.
 *   - Sesion guarda un Usuario* "activo" que apunta a alguno de estos.
 *     Por eso es vector<Usuario*> y no vector<Usuario>: una reasignacion
 *     del vector por valor invalidaria el puntero de Sesion.
 *
 * IDs:
 *   - Auto-incrementales. Si eliminas el id 2, el proximo crear() devuelve 3.
 *   - El contador 'proximoId' se persiste en el archivo para sobrevivir reinicios.
 *
 * Persistencia:
 *   - cargarDesdeArchivo se llama al arrancar la app.
 *   - guardarEnArchivo se llama AUTOMATICAMENTE dentro de crear() y eliminar().
 *   - Tambien la debe llamar la UI despues de un logout (cuando se consolidan
 *     los likes / watchLater / generos del usuario activo).
 */
class GestionUsuarios {
public:
    static const size_t MAX_USUARIOS = 4;

private:
    std::vector<Usuario*> usuarios;
    int proximoId;

    GestionUsuarios();

public:
    static GestionUsuarios& getInstance();

    GestionUsuarios(const GestionUsuarios&) = delete;
    GestionUsuarios& operator=(const GestionUsuarios&) = delete;

    ~GestionUsuarios();

    // Gestion de perfiles

    // Crea un nuevo perfil con id auto-asignado.
    // Devuelve el Usuario* recien creado, o nullptr si ya hay MAX_USUARIOS.
    // Persiste al disco automaticamente.
    Usuario* crear(const std::string& nombre);

    // Elimina el perfil con el id dado.
    // Si era el usuario activo en Sesion, hace logout primero para evitar
    // que Sesion quede con un puntero colgante.
    // Devuelve true si elimino algo, false si no encontro el id.
    // Persiste al disco automaticamente.
    bool eliminar(int id);

    // Devuelve el Usuario* con ese id, o nullptr si no existe.
    // No hace login automatico: la UI llama a Sesion::login() despues.
    Usuario* seleccionar(int id);

    // Lista (id, nombre) de todos los perfiles, en orden de creacion.
    // Para que la UI imprima el menu de seleccion.
    std::vector<std::pair<int, std::string>> listar() const;

    bool estaLleno() const;
    bool estaVacio() const;
    size_t cantidad() const;

    // Persistencia

    // Carga perfiles desde disco. Reemplaza el estado actual.
    // Si habia un usuario activo en Sesion, hace logout primero.
    // Si el archivo no existe (primer arranque) devuelve false sin error.
    // Si el archivo existe pero esta corrupto, deja el estado intacto
    //   (no carga parcialmente) y devuelve false.
    bool cargarDesdeArchivo(const std::string& ruta = "perfiles.txt");

    // Vuelca todos los perfiles a disco.
    // Devuelve true si la escritura fue exitosa.
    bool guardarEnArchivo(const std::string& ruta = "perfiles.txt") const;

    // Utilidades (principalmente para tests)
    // Libera todos los Usuario*, vacia el vector y resetea proximoId a 0.
    // No toca el archivo. Tambien hace logout en Sesion por seguridad.
    void limpiarTodo();
};

#endif