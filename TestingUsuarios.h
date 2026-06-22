#ifndef PROYECTO_PROGRA_3_TESTING_USUARIOS_H
#define PROYECTO_PROGRA_3_TESTING_USUARIOS_H
#include "GestionUsuarios.h"
#include "Sesion.h"
#include "Usuario.h"
#include "UsuarioHistorial.h"
#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

// Helpers de los tests

namespace tu_detail {
    // Una funcion en vez de variable inline para compatibilidad con C++14
    inline const char* archivoTest() { return "perfiles_test.txt"; }

    inline void resetTodo() {
        // Limpia singletons y borra el archivo de test
        GestionUsuarios::getInstance().limpiarTodo();
        Sesion::getInstance().logout();
        std::remove(tu_detail::archivoTest());
    }
}

// Tests

inline void testCrearYListar() {
    tu_detail::resetTodo();
    auto& g = GestionUsuarios::getInstance();

    assert(g.estaVacio());
    assert(g.cantidad() == 0);

    Usuario* u1 = g.crear("Sandra Sofia");
    assert(u1 != nullptr);
    assert(u1->getId() == 0);
    assert(u1->getNombre() == "Sandra Sofia");

    Usuario* u2 = g.crear("Marcelo Daniel");
    Usuario* u3 = g.crear("Denilson");
    Usuario* u4 = g.crear("Santiago");
    assert(u2 && u3 && u4);
    assert(u4->getId() == 3);

    assert(g.estaLleno());
    assert(g.cantidad() == 4);

    // 5to perfil rechazado
    Usuario* u5 = g.crear("Enrique");
    assert(u5 == nullptr);
    assert(g.cantidad() == 4);

    // Lista en orden de creacion
    auto lista = g.listar();
    assert(lista.size() == 4);
    assert(lista[0].first == 0 && lista[0].second == "Sandra Sofia");
    assert(lista[3].first == 3 && lista[3].second == "Santiago");

    std::cout << "[OK] testCrearYListar" << std::endl;
}

inline void testEliminarYIdsNoReciclados() {
    tu_detail::resetTodo();
    auto& g = GestionUsuarios::getInstance();

    g.crear("A"); // id 0
    g.crear("B"); // id 1
    g.crear("C"); // id 2

    // Eliminar el del medio
    assert(g.eliminar(1));
    assert(g.cantidad() == 2);
    assert(g.seleccionar(1) == nullptr);
    assert(g.seleccionar(0) != nullptr);
    assert(g.seleccionar(2) != nullptr);

    // El siguiente crear() NO recicla el 1, da el 3
    Usuario* nuevo = g.crear("D");
    assert(nuevo != nullptr);
    assert(nuevo->getId() == 3);

    // Eliminar inexistente
    assert(!g.eliminar(999));

    std::cout << "[OK] testEliminarYIdsNoReciclados" << std::endl;
}

inline void testEliminarUsuarioActivoHaceLogout() {
    tu_detail::resetTodo();
    auto& g = GestionUsuarios::getInstance();
    auto& s = Sesion::getInstance();

    Usuario* u = g.crear("Activo");
    assert(s.login(u));
    assert(s.getActivo() == u);

    // Al eliminar al activo, Sesion debe quedar sin activo
    assert(g.eliminar(u->getId()));
    assert(s.getActivo() == nullptr);
    assert(!s.hayUsuarioActivo());

    std::cout << "[OK] testEliminarUsuarioActivoHaceLogout" << std::endl;
}

inline void testSeleccionarYLoginManual() {
    tu_detail::resetTodo();
    auto& g = GestionUsuarios::getInstance();
    auto& s = Sesion::getInstance();

    g.crear("Sandra");
    g.crear("Marcelo");

    Usuario* sel = g.seleccionar(1);
    assert(sel != nullptr);
    assert(sel->getNombre() == "Marcelo");

    // seleccionar() NO hace login. Es la UI quien decide.
    assert(!s.hayUsuarioActivo());

    // Login explicito
    assert(s.login(sel));
    assert(s.getActivo() == sel);

    // No se puede login con otro sin logout primero (Sesion::login spec)
    Usuario* otro = g.seleccionar(0);
    assert(!s.login(otro));

    std::cout << "[OK] testSeleccionarYLoginManual" << std::endl;
}

inline void testPersistenciaCompleta() {
    tu_detail::resetTodo();
    auto& g = GestionUsuarios::getInstance();

    // Construir estado con datos en likes/watchLater/generos
    Usuario* a = g.crear("Sandra Sofia");
    a->darLikeYRegistrarGeneros(101, {"action", "crime"});
    a->darLikeYRegistrarGeneros(202, {"action"});
    a->agregarVerMasTarde(303);
    a->agregarVerMasTarde(404);

    Usuario* b = g.crear("Marcelo Daniel");
    b->darLikeYRegistrarGeneros(500, {"romantic comedy", "drama"});

    // Forzar un guardado en el archivo de test (los crear() guardaron en el
    // default "perfiles.txt"; queremos uno controlado).
    assert(g.guardarEnArchivo(tu_detail::archivoTest()));

    // Limpiar y recargar
    g.limpiarTodo();
    assert(g.estaVacio());

    assert(g.cargarDesdeArchivo(tu_detail::archivoTest()));
    assert(g.cantidad() == 2);

    // Verificar Sandra
    Usuario* sandraReloaded = g.seleccionar(0);
    assert(sandraReloaded != nullptr);
    assert(sandraReloaded->getNombre() == "Sandra Sofia");
    assert(sandraReloaded->tieneLike(101));
    assert(sandraReloaded->tieneLike(202));
    assert(!sandraReloaded->tieneLike(303)); // 303 era watchLater, no like
    assert(sandraReloaded->estaEnVerMasTarde(303));
    assert(sandraReloaded->estaEnVerMasTarde(404));

    const auto& gens = sandraReloaded->getHistorial()->getGenerosFavoritos();
    // action aparecio 2 veces (en 2 likes diferentes), crime 1 vez
    assert(gens.at("action") == 2);
    assert(gens.at("crime")  == 1);

    // Verificar Marcelo: genero con espacio se conserva
    Usuario* marceloReloaded = g.seleccionar(1);
    assert(marceloReloaded != nullptr);
    assert(marceloReloaded->getNombre() == "Marcelo Daniel");
    const auto& gensB = marceloReloaded->getHistorial()->getGenerosFavoritos();
    assert(gensB.at("romantic comedy") == 1);
    assert(gensB.at("drama") == 1);

    std::cout << "[OK] testPersistenciaCompleta" << std::endl;
}

inline void testProximoIdSobreviveAlReinicio() {
    tu_detail::resetTodo();
    auto& g = GestionUsuarios::getInstance();

    g.crear("A"); // 0
    g.crear("B"); // 1
    g.crear("C"); // 2
    g.eliminar(1);
    // proximoId debe ser 3 ahora

    assert(g.guardarEnArchivo(tu_detail::archivoTest()));
    g.limpiarTodo();
    assert(g.cargarDesdeArchivo(tu_detail::archivoTest()));

    // Tras recargar, el siguiente crear() debe dar 3, no 2 ni 1
    Usuario* nuevo = g.crear("D");
    assert(nuevo != nullptr);
    assert(nuevo->getId() == 3);

    std::cout << "[OK] testProximoIdSobreviveAlReinicio" << std::endl;
}

inline void testCargarArchivoInexistente() {
    tu_detail::resetTodo();
    auto& g = GestionUsuarios::getInstance();

    g.crear("Pre");
    assert(g.cantidad() == 1);

    // Archivo que no existe -> false, sin alterar el estado actual
    bool ok = g.cargarDesdeArchivo("no_existe_xyz_12345.txt");
    assert(!ok);
    assert(g.cantidad() == 1); // estado intacto
    assert(g.seleccionar(0) != nullptr);

    std::cout << "[OK] testCargarArchivoInexistente" << std::endl;
}

inline void testCargarArchivoCorruptoNoAlteraEstado() {
    tu_detail::resetTodo();
    auto& g = GestionUsuarios::getInstance();

    g.crear("EstadoPrevio");
    assert(g.cantidad() == 1);

    // Escribir un archivo basura
    {
        std::ofstream f(tu_detail::archivoTest());
        f << "esto no es un archivo valido\n";
        f << "random garbage\n";
    }

    bool ok = g.cargarDesdeArchivo(tu_detail::archivoTest());
    assert(!ok);
    // El estado previo no debe haberse perdido
    assert(g.cantidad() == 1);
    assert(g.seleccionar(0) != nullptr);
    assert(g.seleccionar(0)->getNombre() == "EstadoPrevio");

    std::cout << "[OK] testCargarArchivoCorruptoNoAlteraEstado" << std::endl;
}

inline void correrTestsUsuarios() {
    std::cout << "=== Tests Paquete 2 (GestionUsuarios + persistencia) ===" << std::endl;
    testCrearYListar();
    testEliminarYIdsNoReciclados();
    testEliminarUsuarioActivoHaceLogout();
    testSeleccionarYLoginManual();
    testPersistenciaCompleta();
    testProximoIdSobreviveAlReinicio();
    testCargarArchivoInexistente();
    testCargarArchivoCorruptoNoAlteraEstado();
    std::cout << "=== Todos los tests de usuarios pasaron ===" << std::endl;

    // Limpieza final
    tu_detail::resetTodo();
    std::remove("perfiles.txt"); // por los crear() que guardaron en el default
}

#endif