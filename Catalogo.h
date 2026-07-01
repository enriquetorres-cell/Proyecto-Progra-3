#ifndef REPO_PROGRA3_CATALOGO_H
#define REPO_PROGRA3_CATALOGO_H
 
#include "Movie.h"
#include <unordered_map>
 
class Catalogo {
    std::vector<Movie*> movies;

    std::unordered_map<int, Movie*> porId;
 
public:
    Catalogo() = default;
 

    Catalogo(const Catalogo&) = delete;
    Catalogo& operator=(const Catalogo&) = delete;
 
    void agregar(Movie* movie) {
        if (movie != nullptr) {
            movies.push_back(movie);
            porId[movie->id] = movie;
        }
    }
 
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
 
    void limpiar() {
        for (Movie* m : movies) delete m;
        movies.clear();
        porId.clear();
    }
 
    ~Catalogo() {
        for (Movie* m : movies) delete m;
    }
};
 
#endif 