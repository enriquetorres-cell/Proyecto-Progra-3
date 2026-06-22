#ifndef REPO_PROGRA3_CATALOGO_H
#define REPO_PROGRA3_CATALOGO_H
 
#include "Movie.h"
#include <unordered_map>
 
class Catalogo {
    std::vector<Movie*> movies;
    // FIX Error 4: indice por id para getById en O(1).
    // Se mantiene en paralelo a 'movies' en agregar() y limpiar().
    std::unordered_map<int, Movie*> porId;
 
public:
    Catalogo() = default;
 
    // FIX Error 8: el Catalogo es dueño de las Movie*.
    // Prohibir la copia evita doble delete al destruir copias.
    Catalogo(const Catalogo&) = delete;
    Catalogo& operator=(const Catalogo&) = delete;
 
    void agregar(Movie* movie) {
        if (movie != nullptr) {
            movies.push_back(movie);
            porId[movie->id] = movie;
        }
    }
 
    // FIX Error 4: O(1) usando el indice.
    Movie* getById(int movieId) {
        auto it = porId.find(movieId);
        return it == porId.end() ? nullptr : it->second;
    }
 
    const Movie* getById(int movieId) const {
        auto it = porId.find(movieId);
        return it == porId.end() ? nullptr : it->second;
    }
 
    const std::vector<Movie*>& getMovies() const {
        return movies;
    }
 
    size_t size() const {
        return movies.size();
    }
 
    bool estaVacio() const {
        return movies.empty();
    }
 
    // FIX Error 3: libera las Movie* antes de vaciar.
    void limpiar() {
        for (Movie* m : movies) delete m;
        movies.clear();
        porId.clear();
    }
 
    // FIX Error 3: el destructor libera todas las Movie*.
    ~Catalogo() {
        for (Movie* m : movies) delete m;
    }
};
 
#endif 