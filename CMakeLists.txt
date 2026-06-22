cmake_minimum_required(VERSION 4.0)
project(Proyecto_Progra_3)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/index)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/search)
# Fuentes comunes (foundation + paquete 3 + paquete 2)
set(COMMON_SOURCES
        dataframe.cpp
        tokenizador.cpp
        index/Trie.cpp
        index/NgramIndex.cpp
        index/Indexador.cpp
        GestionUsuarios.cpp
)

# ============================================================
# 1) App principal
# ============================================================
add_executable(Proyecto_Progra_3
        main.cpp
        ${COMMON_SOURCES}
        index/main_test.cpp
        index/Testing.h
        Sesion.h
        GestionUsuarios.h
        GestionUsuarios.h
        GestionUsuarios.h
        GestionUsuarios.cpp
        GestionUsuarios.cpp
        TestingUsuarios.h
        test_usuarios_main.cpp
)

# ============================================================
# 2) Tests del Paquete 3 (Trie + NgramIndex + Indexador)
#    Ejecutar:   ./test_indices
# ============================================================
add_executable(test_indices
        index/main_test.cpp
        ${COMMON_SOURCES}
)

# ============================================================
# 3) Tests del Paquete 2 (GestionUsuarios + persistencia)
#    Ejecutar:   ./test_usuarios
# ============================================================
add_executable(test_usuarios
        test_usuarios_main.cpp
        GestionUsuarios.cpp
)

# ============================================================
# 4) Prueba de lectura del CSV (archivo legacy, autocontenido)
# ============================================================
add_executable(test_lectura
        Prueba_lectura.cpp
)

# ============================================================
# 5) Tests del Paquete 4 (Busqueda + Ranking + Composite)
#    Ejecutar:   ./test_busqueda
# ============================================================
set(SEARCH_SOURCES
        search/Query.cpp
        search/Ranker.cpp
        search/Buscador.cpp
)
add_executable(test_busqueda
        search/main_test_busqueda.cpp
        ${SEARCH_SOURCES}
        ${COMMON_SOURCES}
)