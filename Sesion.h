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
 

    bool login(Usuario* usuario) {
        if (usuario == nullptr) return false;
        if (usuarioActivo != nullptr) return false;
        usuarioActivo = usuario;
        return true;
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
 
#endif