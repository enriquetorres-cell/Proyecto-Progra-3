#ifndef REPO_PROGRA3_USUARIO_H
#define REPO_PROGRA3_USUARIO_H
 
#include <string>
#include <vector>
#include "UsuarioHistorial.h"
 
class Usuario {
    int id = 0;
    std::string nombre;
    UsuarioHistorial* historial;
 
public:
    Usuario() {
        this->id = 0;
        this->nombre = "";
        this->historial = new UsuarioHistorial();
    }
 
    Usuario(int id, const std::string& nombre) {
        this->id = id;
        this->nombre = nombre;
        this->historial = new UsuarioHistorial();
    }
 
    // FIX Error 1: deep copy. Antes se copiaba el puntero -> doble delete.
    Usuario(const Usuario& other) {
        this->id = other.id;
        this->nombre = other.nombre;
        this->historial = new UsuarioHistorial(*other.historial);
    }
 
    // FIX Error 2: const&.
    // FIX Error 6: logica clara con else.
    Usuario& operator=(const Usuario& other) {
        if (this != &other) {
            this->id = other.id;
            this->nombre = other.nombre;
            if (this->historial == nullptr) {
                this->historial = new UsuarioHistorial(*other.historial);
            } else {
                *this->historial = *other.historial;
            }
        }
        return *this;
    }
 
    int getId() const {
        return this->id;
    }
 
    std::string getNombre() const {
        return this->nombre;
    }
 
    void setNombre(const std::string& newNombre) {
        this->nombre = newNombre;
    }
 
    UsuarioHistorial* getHistorial() {
        return this->historial;
    }
 
    const UsuarioHistorial* getHistorial() const {
        return this->historial;
    }
 
    void darLikeYRegistrarGeneros(int movieId_, const std::vector<std::string>& generos) {
        historial->agregarLike(movieId_);
        historial->registrarGeneros(generos);
    }
 
    void quitarLike(int movieId_) {
        historial->quitarLike(movieId_);
    }
 
    bool tieneLike(int movieId_) const {
        return historial->tieneLike(movieId_);
    }
 
    void agregarVerMasTarde(int movieId_) {
        historial->agregarVerMasTarde(movieId_);
    }
 
    void quitarVerMasTarde(int movieId_) {
        historial->quitarVerMasTarde(movieId_);
    }
 
    bool estaEnVerMasTarde(int movieId_) const {
        return historial->estaEnVerMasTarde(movieId_);
    }
 
    ~Usuario() {
        delete this->historial;
    }
};
 
#endif