//
// Created by Santiago Davila on 20/06/26.
//

#ifndef REPO_PROGRA3_CATALOGO_H
#define REPO_PROGRA3_CATALOGO_H
#include "Movie.h"

class Catalogo {
    vector<Movie*> movies;
public:
    Catalogo() = default;
    void agregar(Movie* movie) {
        if (movie != nullptr) {
            movies.push_back(movie);
        }
    }
    Movie* getById(int movieId) {
        for (Movie* movie: movies) {
            if (movie != nullptr && movie->id == movieId) {
                return movie;
            }
        }
        return nullptr;
    }
    const Movie* getById(int movieId) const {
        for (Movie* movie: movies) {
            if (movie != nullptr && movie->id == movieId) {
                return movie;
            }
        }
        return nullptr;
    }
    const vector<Movie*>& getMovies() const {
        return movies;
    }
    size_t size() {
        return movies.size();
    }
    bool estaVacio() const {
        return movies.empty();
    }
    void limpiar() {
        movies.clear();
    }
    ~Catalogo() = default;
};


#endif //REPO_PROGRA3_CATALOGO_H