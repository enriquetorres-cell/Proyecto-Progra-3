//
// Created by Santiago Davila on 20/06/26.
//

#ifndef REPO_PROGRA3_SESION_H
#define REPO_PROGRA3_SESION_H
#include "Usuario.h"

class Sesion {
    Usuario* usuarioActivo;
    Sesion() {
        usuarioActivo = nullptr;
    }
public:
    static Sesion& getInstance() {
        static Sesion instance;
        return instance;
    }
    void login(Usuario* usuario) {
        usuarioActivo = usuario;
    }
    void logout() {
        usuarioActivo = nullptr;
    }
    Usuario* getActivo() {
        return usuarioActivo;
    }
    const Usuario* getActivo() const {
        return usuarioActivo;
    }
    bool hayUsuarioActivo() const {
        return usuarioActivo != nullptr;
    }
    Sesion(const Sesion&) = delete;
    Sesion& operator=(const Sesion&) = delete;
};


#endif //REPO_PROGRA3_SESION_H