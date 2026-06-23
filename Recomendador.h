//
// Created by marce on 23/06/2026.
//

#ifndef PROYECTO_PROGRA_3_RECOMENDADOR_H
#define PROYECTO_PROGRA_3_RECOMENDADOR_H
#include "Catalogo.h"
#include "UsuarioHistorial.h"
#include <vector>
#include <string>
#include <map>
using namespace std;


class Recomendador {
    const Catalogo& catalogo;
    int pesoLike = 2;
    int pesoWatchLater = 1;
public:
    explicit Recomendador(const Catalogo& cat);
    vector<int> recomendar(const UsuarioHistorial& historial, int topN = 10) const;
    private:
    map<string, int> calcularPesosPorGenero(const UsuarioHistorial& h) const;
    bool yaVista(int movieId, const UsuarioHistorial& h) const;
    int puntajePelicula(const Movie* peli, const map<string, int>& pesos) const;
};


#endif //PROYECTO_PROGRA_3_RECOMENDADOR_H