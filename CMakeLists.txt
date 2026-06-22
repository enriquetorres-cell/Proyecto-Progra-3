cmake_minimum_required(VERSION 4.0)
project(Proyecto_Progra_3)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/index)
# Fuentes comunes (foundation + paquete 3)
set(COMMON_SOURCES
        dataframe.cpp
        tokenizador.cpp
        index/Trie.cpp
        index/NgramIndex.cpp
        index/Indexador.cpp
)

# ============================================================
# 1) App principal
# ============================================================
add_executable(Proyecto_Progra_3
        main.cpp
        ${COMMON_SOURCES}
        index/main_test.cpp
        index/Testing.h
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
# 3) Prueba de lectura del CSV (archivo legacy, autocontenido)
# ============================================================
add_executable(test_lectura
        Prueba_lectura.cpp
)
