# Proyecto-Progra-3
Tema: Se debe implementar un algoritmo de busqueda y visualización

- Marcelo // limpieza del .csv

Cosas faltantes:

- Almacenar like y ver mas tarde
- Visualización
- Implementar busqueda segun el string entrando // "bar" = barco, "barco fantasma" = barco y/o fantasma // o segun el tag del director 
- Algoritmo segun importancia

Estructura de nuestro árbol:
Árbol Trie 
- Se enfoca en guardar y buscar cadenas de texto (string/palabras), cada nodo guarda una letra de la palabra 
- y asi al seguir el camino desde la raiz hacia abajo vas armando la palabra
Desventajas:
- Es el mismo tiempo si tienes 10 o 1 millon de datos, la busqueda depende solo del largo de la palabra

Implementar
carpeta index
Logica del arbol Trie / Trie.h -- Si se necesita se implementan otras clases
Testeo / Testing.h

carpeta core
Implementar clase Usuario 
Implementar clase UsuarioHistorial // Donde ira las peliculas que le gusta, lo de ver mas tarde
Implementar clase pelicula
Implementar clase Sesion
Implementar clase GestionUsuarios // Almacena los 4 usuarios y se almacena memoria de estos en esa clase 
Implementar clase Catalogo // Donde se almacenen todas las peliculas




### 1) Pantalla de perfiles / Sesión
- Se muestran **4 slots** (máximo 4 usuarios).
- Solo puede existir **1 usuario activo por sesión** (enforced por `Session`).
- Para **cambiar de perfil**, primero se **cierra sesión** (logout) y se selecciona otro slot.

### 2) Home
- Muestra recomendaciones personalizadas.
- Muestra lista “Ver más tarde”.
- Muestra categorías (top tags).
- Acceso a búsqueda.

### 3) Búsqueda
La búsqueda soporta:
- **Búsqueda normal**
    - Ej: `batman`, `space travel`, `love story`
- **Búsqueda por tag, genero, director, etc**
    - Ej: `tag:horror`, `tag:action`
- **Búsqueda por fragmentos (substring)**
    - Soportada por `NgramIndex` (n-grams).



