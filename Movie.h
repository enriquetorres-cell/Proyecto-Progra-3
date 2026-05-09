

#ifndef AVANCE1_MOVIE_H
#define AVANCE1_MOVIE_H
#pragma once
#include <vector>
#include <string>
using namespace std;

struct Movie {
    int id;
    int releaseYear;
    string title;
    string origin;
    string director;
    vector<string> cast;
    vector<string> genres;
    string plot;};

using dataframe = vector<Movie*>;

#endif //AVANCE1_MOVIE_H