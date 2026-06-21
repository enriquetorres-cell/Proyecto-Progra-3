//
// Created by Santiago Davila on 20/06/26.
//

#ifndef REPO_PROGRA3_USUARIO_H
#define REPO_PROGRA3_USUARIO_H
#include <string>
#include <vector>
#include "UsuarioHistorial.h"
class Usuario {
    int id = 0;
    string nombre;
    UsuarioHistorial* historial;
public:
    Usuario() {
        this->id = 0;
        this->nombre = "";
        this->historial = new UsuarioHistorial();
    }
    Usuario(int id, const string& nombre) {
        this->id = id;
        this->nombre = nombre;
        this->historial = new UsuarioHistorial();
    }
    Usuario(const Usuario& other) {
        this->id = other.id;
        this->nombre = other.nombre;
        this->historial = other.historial;
    }
    Usuario& operator=(Usuario& other) {
        if (this != &other) {
            this->id = other.id;
            this->nombre = other.nombre;
            if (this->historial == nullptr) {
                this->historial = new UsuarioHistorial(*other.historial);
            }
            *this->historial = *other.historial;
        }
        return *this;
    }
    int getId() {
        return this->id;
    }
    string getNombre() {
        return this->nombre;
    }
    void setNombre(const string& newNombre) {
        this->nombre = newNombre;
    }
    UsuarioHistorial* getHistorial() {
        return this->historial;
    }
    const UsuarioHistorial* getHistorial() const {
        return this->historial;
    }
    void darLikeYRegistrarGeneros(int movieId_, const vector<string>& generos) {
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
    };
};


#endif //REPO_PROGRA3_USUARIO_H