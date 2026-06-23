cmake_minimum_required(VERSION 4.0)
project(Proyecto_Progra_3)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/index)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/search)

set(COMMON_SOURCES
        dataframe.cpp
        tokenizador.cpp
        index/Trie.cpp
        index/Ngramindex.cpp
        index/Indexador.cpp
        GestionUsuarios.cpp
        Recomendador.cpp)

set(SEARCH_SOURCES
        search/Query.cpp
        search/Buscador.cpp
        search/RankingHibrido.cpp
        search/RelevanciaTextual.cpp
        search/AfinidadConLikes.cpp)

# 1) App principal
add_executable(Proyecto_Progra_3
        main.cpp
        ${COMMON_SOURCES}
        ${SEARCH_SOURCES})

# 2) Tests Paquete 3
add_executable(test_indices
        index/main_test.cpp
        ${COMMON_SOURCES})

# 3) Tests Paquete 2
add_executable(test_usuarios
        test_usuarios_main.cpp
        GestionUsuarios.cpp)

# 4) Prueba de lectura
add_executable(test_lectura
        Prueba_lectura.cpp)

# 5) Tests Paquete 4 + 5
add_executable(test_busqueda
        search/main_test_busqueda.cpp
        ${COMMON_SOURCES}
        ${SEARCH_SOURCES})