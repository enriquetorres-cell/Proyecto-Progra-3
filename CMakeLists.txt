cmake_minimum_required(VERSION 4.0)
project(Proyecto_Progra_3)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
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
        search/AfinidadConLikes.cpp
        search/RankingUtils.h)

# 1) App principal
add_executable(Proyecto_Progra_3
        main.cpp
        ${COMMON_SOURCES}
        ${SEARCH_SOURCES})

target_link_libraries(Proyecto_Progra_3 PRIVATE Threads::Threads)
