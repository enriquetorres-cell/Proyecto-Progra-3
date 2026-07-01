# Programación III: Proyecto Final (2026-1)

## Integrantes
- Sandra Sofia Alvarez Lovera
- Denilson Jermai Cuadros Villegas
- Marcelo Daniel Castillo Escudero
- Santiago Sebastian Davila Bazan
- Enrique Eusebio Torres Chafloque

Curso: Programación III, UTEC. 

## Descripción del proyecto

Plataforma de streaming en C++ que administra la búsqueda y visualización de películas a partir de un dataset en `.csv`. El sistema permite:

- Cargar y limpiar el dataset de películas.
- Buscar por palabra, frase o sub-palabra (por ejemplo, "bar" encuentra "barco").
- Buscar por tag: director, género, casting.
- Ordenar los resultados de una búsqueda por relevancia.
- Marcar películas como "Like" o guardarlas en "Ver más tarde".
- Recomendar películas según el historial del usuario activo.

## Estructura del repositorio

```
.
├── main.cpp                 Punto de entrada, interfaz de consola
├── dataframe.h / .cpp       Carga y limpieza del CSV
├── tokenizador.h / .cpp     Tokenización, n-gramas, stopwords
├── Catalogo.h                Contenedor de todas las películas (por id)
├── Movie.h                   Struct Movie + alias dataframe
├── Usuario.h / UsuarioHistorial.h   Perfil y su historial (likes, ver más tarde, vistas)
├── Sesion.h                  Singleton: usuario activo
├── GestionUsuarios.h / .cpp  Singleton: los 4 perfiles + persistencia en disco
├── Recomendador.h / .cpp     Recomendaciones para el Home
├── RankingUtils.h            Utilidad genérica de ordenamiento por puntaje
├── index/
│   ├── Trie.h / .cpp          Árbol elegido para la búsqueda
│   ├── Ngramindex.h / .cpp    Índice de n-gramas para sub-palabras
│   └── Indexador.h / .cpp     Arma el Trie y el NgramIndex a partir del Catalogo
└── search/
    ├── Query.h / .cpp             Jerarquía de consultas (patrón Composite)
    ├── Buscador.h / .cpp          Orquesta Trie + NgramIndex + ranking
    ├── RankingStrategy.h          Interfaz del patrón Strategy
    ├── RankingHibrido.h / .cpp    Estrategia: texto + géneros favoritos
    ├── RelevanciaTextual.h / .cpp Estrategia: solo coincidencia textual
    └── AfinidadConLikes.h / .cpp  Estrategia: prioriza afinidad con los likes
```

## Pipeline de datos

### 1. Carga y limpieza del dataset (`dataframe.cpp`)

`guardarDF` lee el `.csv`, separa cada línea en columnas, arma un `Movie*` por fila y limpia el contenido: recorta espacios sobrantes, pasa a minúsculas, normaliza separadores (`/`, `&`, "and") y descarta valores como "unknown" o "desconocido". También elimina duplicados por `(título, año, origen)` y descarta filas sin año de estreno válido. El resultado se guarda en `dataframe` (`vector<Movie*>`).

### 2. Preprocesamiento e indexado (`tokenizador.cpp` + `index/Indexador.cpp`)

Se carga la lista de palabras despreciables desde un `.txt`, se tokeniza el título y la sinopsis de cada película, se descartan tokens de menos de 2 caracteres y las palabras despreciables, y se generan trigramas por token. Con esto se construyó un vocabulario de 91,980 palabras únicas, que es lo que se inserta en el Trie y en el `NgramIndex`. También se indexan tags especiales (`director:...`, `tag:...`).

## Árbol elegido: Trie

| | Ventaja | Desventaja |
|---|---|---|
| Trie | Búsqueda en O(L), donde L es el largo de la palabra: el mismo costo con 10 o con 1 millón de películas. Ideal para prefijos y autocompletado. | Consume más memoria que una tabla hash si el vocabulario es muy grande. No resuelve por sí solo la búsqueda por sub-palabra. |

Complejidad:

| Operación | Costo | Detalle |
|---|---|---|
| Inserción | O(L) | Un nodo por carácter de la palabra |
| Búsqueda exacta | O(L) | Se desciende carácter a carácter hasta el nodo final |
| Búsqueda por prefijo | O(L) + O(k) | Igual que la exacta, más un DFS sobre el subárbol para recolectar ids (k = tamaño del subárbol) |
| Espacio | O(N·L) | N = número de palabras, L = largo promedio |

Para resolver la búsqueda por sub-palabra (donde el Trie no es eficiente por sí solo), se complementa con un `NgramIndex` de trigramas: `"barco"` se indexa también como `"bar"`, `"arc"`, `"rco"`, lo que permite encontrar `"barco"` al buscar `"bar"`.

## Búsqueda (`search/`)

`Buscador::buscar` separa la consulta del usuario en tokens. Cada token se convierte en un objeto `Query`:

- `TermQuery`: palabra normal, consulta el Trie (exacto + prefijo) y el `NgramIndex`.
- `TagQuery` / `DirectorQuery`: tokens con el formato `tag:genero` o `director:nombre`, consultan el Trie por clave exacta.

Todos los `Query` se agregan a un `OrQuery`, que evalúa a sus hijos y une los resultados (patrón **Composite**: `OrQuery` y las hojas comparten la interfaz `Query::evaluar`, así que el `Buscador` no necesita distinguir entre un término simple y una combinación de varios). El conjunto de ids resultante se pasa a una `RankingStrategy` para ordenarlo.

## Ranking y recomendación

`RankingStrategy` es la interfaz del patrón **Strategy**: el `Buscador` recibe la estrategia por referencia y no conoce los detalles de cómo se calcula el puntaje. Hay tres implementaciones intercambiables:

- **RankingHibrido**: puntúa coincidencias en título (3), director (2) y sinopsis (1); suma 2 puntos si la película comparte género con los favoritos del usuario.
- **RelevanciaTextual**: igual esquema de puntos, pero exige coincidencia de palabra completa (no substring) y no usa el historial del usuario; desempata por año de estreno.
- **AfinidadConLikes**: pondera más el historial de géneros favoritos que el texto.

`Recomendador` arma la sección de recomendaciones del Home: calcula un peso por género combinando likes (peso 3), ver más tarde (peso 2) y vistas recientes (peso 1), puntúa el catálogo completo con esos pesos y, si el usuario no tiene historial, cae a `peliculasMasRecientes`.

## Programación genérica

El cálculo de puntaje difiere entre las estrategias de ranking (`double`, con distintos criterios de desempate) y el recomendador (`int`, comparando también por id). Ordenar esos candidatos y quedarse con los primeros N ids es la misma operación en los cinco casos, solo cambia el tipo del puntaje y el criterio de desempate. Esa lógica vive en una única función template, en `RankingUtils.h`:

```cpp
template <typename Score, typename Comparador>
vector<int> ordenarPorPuntaje(vector<pair<int, Score>> candidatos,
                               Comparador comparador,
                               int topN = -1);
```

`Score` varía según quien la llame (`double` en `RankingHibrido`, `RelevanciaTextual` y `AfinidadConLikes`; `int` en `Recomendador`) y `Comparador` recibe el criterio de orden como lambda, para que cada estrategia conserve su propio desempate. La función se usa en las cinco funciones mencionadas y elimina la duplicación sin forzar una única forma de comparar.

## Programación paralela

Dos etapas de la carga recorren todo el dataset fila por fila sin compartir estado entre sí, así que corren en hilos:

1. **`dataframe.cpp::guardarDF`**: se leen todas las líneas del `.csv` a memoria (paso secuencial, es I/O sobre un único `ifstream`) y luego se reparten en bloques entre `std::thread::hardware_concurrency()` hilos. Cada hilo parsea y limpia su bloque hacia su propio `vector<Movie*>`, sin locks. El chequeo de duplicados y la asignación de id se hacen en un merge secuencial al final, recorriendo los bloques en orden para conservar el orden original del archivo.
2. **`index/Indexador.cpp::indexar`**: mismo esquema. Cada hilo tokeniza su bloque de películas hacia un `vector` privado de pares `(token, movieId)`; la inserción en el Trie y el `NgramIndex` (que no son thread-safe) se hace de forma secuencial al final.

### Comparación de tiempos

Medido en la misma máquina, con el mismo dataset (33,416 películas), comparando la rama con hilos (`std::thread::hardware_concurrency()` en `guardarDF` e `Indexador::indexar`) contra la rama sin hilos. El tiempo es el de carga completa (`runApp`: `guardarDF` + `Indexador::indexar` + `GestionUsuarios`), medido con `high_resolution_clock`:

| Versión | Tiempo de carga | Speedup |
|---|---|---|
| Sin hilos | 24.147 s | 1x |
| Con hilos | 11.778 s | ~2.05x |

El tiempo no se aisló por etapa (`guardarDF` vs. `Indexador::indexar` por separado), así que esta tabla mide el efecto conjunto de paralelizar ambas. El speedup real (~2x) queda por debajo del límite teórico de la Ley de Amdahl para la cantidad de hilos disponibles en la máquina, lo esperable dado que la lectura del archivo y el merge de las estructuras compartidas (Trie, NgramIndex, `set` de duplicados) se mantienen secuenciales.

## Patrones de diseño

| Patrón | Dónde | Rol |
|---|---|---|
| Strategy | `search/RankingStrategy.h` + `RankingHibrido`, `RelevanciaTextual`, `AfinidadConLikes` | El algoritmo de ranking es intercambiable sin que `Buscador` cambie |
| Singleton | `Sesion.h`, `GestionUsuarios.h` | Una única instancia de la sesión activa y del gestor de los 4 perfiles en toda la app |
| Composite | `search/Query.h` (`OrQuery` sobre `TermQuery`/`TagQuery`/`DirectorQuery`) | Una consulta compuesta se trata igual que una consulta simple |

## Persistencia

`GestionUsuarios` guarda y carga los 4 perfiles (likes, ver más tarde, vistas recientes, géneros favoritos) en `perfiles.txt` con una estrategia "todo o nada": si el archivo está corrupto, no se carga nada parcialmente y se conserva el estado en memoria.

## Cómo compilar y ejecutar

```bash
mkdir build && cd build
cmake ..
cmake --build .
./Proyecto_Progra_3
```

Requiere C++14 y soporte de `std::thread` (ya vinculado en `CMakeLists.txt` vía `Threads::Threads`).

## Referencias

Cormen, T. H., Leiserson, C. E., Rivest, R. L., & Stein, C. (2022). *Introduction to algorithms* (4th ed.). MIT Press.

Fredkin, E. (1960). Trie memory. *Communications of the ACM*, *3*(9), 490–499. https://doi.org/10.1145/367390.367400

Gamma, E., Helm, R., Johnson, R., & Vlissides, J. (1994). *Design patterns: Elements of reusable object-oriented software*. Addison-Wesley.

Stroustrup, B. (2013). *The C++ programming language* (4th ed.). Addison-Wesley.

Williams, A. (2019). *C++ concurrency in action* (2nd ed.). Manning Publications.

cppreference.com. (n.d.). *std::thread*. Retrieved June 2026, from https://en.cppreference.com/w/cpp/thread/thread
