#include <iostream>
#include <random>
#include <utility>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>
#include <cstdlib>

using namespace std;

int NUM_CIUDADES;
int TAM_POB;
int NUM_GEN;
double PROB_MUT;

// Función para calcular la distancia entre dos ciudades.
double calcular_distancia(pair<double, double> c1, pair<double, double> c2) {
    double dx = std::pow(c1.first - c2.first, 2);
    double dy = std::pow(c1.second - c2.second, 2);
    return std::sqrt(dx + dy);
}

// Función para calcular la distancia total de una solución.
double calcular_distancia_total(vector<int> ruta, vector<pair<double, double>> ciudades) {
    // 1. Inicializar la distancia total a 0.
    // 2. Recorrer la ruta y sumar la distancia entre cada par de ciudades.
    // 3. Retornar la distancia total.
    double distancia_total = 0.0;
    int num_ciudades = (int) ruta.size();
    for (int i = 0; i < num_ciudades; i++) {
        int ciudad_actual = ruta[i];
        int ciudad_siguiente = ruta[(i + 1) % num_ciudades];
        distancia_total += calcular_distancia(ciudades[ciudad_actual], ciudades[ciudad_siguiente]);
    }
    return distancia_total;
}

// Función para generar una población inicial.
vector<vector<int>> generar_poblacion_inicial(int tamano_poblacion, const vector<pair<double, double>> &ciudades) {
    // 1. Crear un vector de vectores de enteros de tamaño tamano_poblacion x num_ciudades.
    // 2. Para cada individuo de la población, generar una ruta aleatoria.
    // 3. Retornar la población.
    int num_ciudades = (int) ciudades.size();
    vector<vector<int>> poblacion(tamano_poblacion, vector<int>(num_ciudades));
    for (int i = 0; i < tamano_poblacion; i++) {
        for (int j = 0; j < num_ciudades; j++) {
            poblacion[i][j] = j;
        }
        shuffle(poblacion[i].begin(), poblacion[i].end(), std::mt19937(std::random_device()()));
    }
    return poblacion;
}

// Función para seleccionar dos padres de la población actual mediante torneo binario.
vector<int> seleccionar_padres(vector<vector<int>> poblacion, const vector<pair<double, double>> &ciudades) {
    // 1. Crear un vector de índices de tamaño igual al tamaño de la población.
    // 2. Mezclar los índices.
    // 3. Seleccionar los dos primeros índices.
    // 4. Calcular la distancia total de cada individuo seleccionado.
    // 5. Retornar los índices de los dos individuos con menor distancia total.
    int tamano_poblacion = (int) poblacion.size();
    vector<int> indices(tamano_poblacion);
    for (int i = 0; i < tamano_poblacion; i++) {
        indices[i] = i;
    }
    shuffle(indices.begin(), indices.end(), mt19937(random_device()()));
    int indice_padre1 = indices[0];
    int indice_padre2 = indices[1];
    double distancia_padre1 = calcular_distancia_total(poblacion[indice_padre1], ciudades);
    double distancia_padre2 = calcular_distancia_total(poblacion[indice_padre2], ciudades);
    for (int i = 2; i < tamano_poblacion; i++) {
        int indice_actual = indices[i];
        double distancia_actual = calcular_distancia_total(poblacion[indice_actual], ciudades);
        if (distancia_actual < distancia_padre1) {
            indice_padre2 = indice_padre1;
            distancia_padre2 = distancia_padre1;
            indice_padre1 = indice_actual;
            distancia_padre1 = distancia_actual;
        } else if (distancia_actual < distancia_padre2) {
            indice_padre2 = indice_actual;
            distancia_padre2 = distancia_actual;
        }
    }
    return {indice_padre1, indice_padre2};
}

// Función para cruzar dos padres y generar un hijo.
vector<int> cruzar(vector<int> padre1, vector<int> padre2) {
    // 1. Crear un vector de enteros de tamaño igual al tamaño de los padres.
    // 2. Seleccionar un punto de corte aleatorio.
    // 3. Copiar la primera parte del padre1 al hijo.
    // 4. Copiar los elementos del padre2 que no estén en el hijo.
    // 5. Retornar el hijo.
    int tamano_padre = (int) padre1.size();
    vector<int> hijo(tamano_padre);
    int punto_corte = rand() % tamano_padre;
    for (int i = 0; i < punto_corte; i++) {
        hijo[i] = padre1[i];
    }
    int indice_hijo = punto_corte;
    for (int i = 0; i < tamano_padre; i++) {
        int ciudad_actual = padre2[i];
        if (find(hijo.begin(), hijo.end(), ciudad_actual) == hijo.end()) {
            hijo[indice_hijo] = ciudad_actual;
            indice_hijo++;
        }
    }
    return hijo;
}

// Función para mutar un individuo.
void mutar(vector<int> &individuo) {
    // 1. Seleccionar dos posiciones aleatorias.
    // 2. Intercambiar los elementos de las posiciones seleccionadas.
    int tamano_individuo = (int) individuo.size();
    int pos1 = rand() % tamano_individuo;
    int pos2 = rand() % tamano_individuo;
    swap(individuo[pos1], individuo[pos2]);
}

// Función para ejecutar una iteración del algoritmo genético
vector<vector<int>>
ejecutar_iteracion(vector<vector<int>> poblacion, const vector<pair<double, double>> &ciudades, double prob_mutacion) {
    // 1. Crear un vector de vectores de enteros de tamaño igual al tamaño de la población.
    // 2. Para cada individuo de la población:
    //    2.1. Seleccionar dos padres.
    //    2.2. Cruzar los padres y generar un hijo.
    //    2.3. Si se cumple la probabilidad de mutación, mutar el hijo.
    //    2.4. Agregar el hijo al vector de vectores de enteros.
    // 3. Retornar el vector de vectores de enteros.
    int tamano_poblacion = (int) poblacion.size();
    vector<vector<int>> nueva_poblacion(tamano_poblacion, vector<int>(ciudades.size()));
    for (int i = 0; i < tamano_poblacion; i++) {
        vector<int> padres = seleccionar_padres(poblacion, ciudades);
        vector<int> hijo = cruzar(poblacion[padres[0]], poblacion[padres[1]]);
        if (static_cast<double>(rand()) / RAND_MAX < prob_mutacion) {
            mutar(hijo);
        }
        nueva_poblacion[i] = hijo;
    }
    return nueva_poblacion;
}

// Función Calcular Fitness
double calcular_fitness(vector<int> solucion, vector<pair<double, double>> ciudades) {
    // 1. Calcular la distancia total de la solución.
    // 2. Retornar el inverso de la distancia total.
    double distancia_total = calcular_distancia_total(std::move(solucion), std::move(ciudades));
    return 1.0 / distancia_total;
}

// Función para ejecutar el algoritmo genético.
vector<vector<int>>
ejecutar_algoritmo_genetico(const vector<pair<double, double>> &ciudades, int tamano_poblacion, int num_generaciones,
                            double prob_mutacion) {
    // 1. Generar una población inicial.
    // 2. Calcular el fitness de cada individuo de la población.
    // 3. Para cada generación:
    //    3.1. Ejecutar una iteración del algoritmo genético.
    //    3.2. Calcular el fitness de cada individuo de la población.
    //    3.3. Seleccionar las mejores soluciones.
    // 4. Retornar las mejores soluciones.
    vector<vector<int>> poblacion = generar_poblacion_inicial(tamano_poblacion, ciudades);
    vector<double> fitness_poblacion(tamano_poblacion);
    for (int i = 0; i < tamano_poblacion; i++) {
        fitness_poblacion[i] = calcular_fitness(poblacion[i], ciudades);
    }
    vector<vector<int>> mejores_soluciones;
    for (int i = 0; i < num_generaciones; i++) {
        poblacion = ejecutar_iteracion(poblacion, ciudades, prob_mutacion);
        for (int j = 0; j < tamano_poblacion; j++) {
            fitness_poblacion[j] = calcular_fitness(poblacion[j], ciudades);
        }
        int indice_mejor_solucion = (int) (max_element(fitness_poblacion.begin(), fitness_poblacion.end()) -
                                           fitness_poblacion.begin());
        mejores_soluciones.push_back(poblacion[indice_mejor_solucion]);
    }
    return mejores_soluciones;
}


// Función principal para resolver el problema del agente viajero con algoritmo genético
void resolver_problema_agente_viajero(const vector<pair<double, double>> &ciudades, int tamano_poblacion,
                                      int num_generaciones, double prob_mutacion = 0.1) {
    // 1. Definir el tamaño de la población.
    // 2. Definir el número de generaciones.
    // 3. Definir la probabilidad de mutación.
    // 4. Ejecutar el algoritmo genético.
    // 5. Imprimir la mejor solución.
    vector<vector<int>> mejores_soluciones = ejecutar_algoritmo_genetico(ciudades, tamano_poblacion, num_generaciones,
                                                                         prob_mutacion);
    for (const auto &i: mejores_soluciones) {
        cout << calcular_distancia_total(i, ciudades);
        cout << endl;
    }

    //  int indice_mejor_solucion = (int) (max_element(mejores_soluciones.begin(), mejores_soluciones.end()) - mejores_soluciones.begin());
    //  vector<int> mejor_solucion = mejores_soluciones[indice_mejor_solucion];
    vector<int> mejor_solucion = mejores_soluciones[mejores_soluciones.size() - 1];
    cout << "Mejor solucion: ";
    for (int i: mejor_solucion) {
        cout << i << " ";
    }
    cout << endl;
    cout << "Distancia total de la mejor solucion: " << calcular_distancia_total(mejor_solucion, ciudades) << endl;
}

// Función para generar las ciudades aleatorias.
vector<pair<double, double>> generar_ciudades_aleatorias(int num_ciudades) {
    // 1. Crear un vector de pares de doubles de tamaño igual al número de ciudades.
    // 2. Para cada ciudad:
    //    2.1. Generar una coordenada x aleatoria.
    //    2.2. Generar una coordenada y aleatoria.
    //    2.3. Agregar el par de coordenadas al vector de pares de doubles.
    // 3. Retornar el vector de pares de doubles.
    vector<pair<double, double>> ciudades(num_ciudades);
    for (int i = 0; i < num_ciudades; i++) {
        double x = static_cast<double>(rand()) / RAND_MAX;
        double y = static_cast<double>(rand()) / RAND_MAX;
        ciudades[i] = {x, y};
    }
    return ciudades;
}

int main() {
    // 1. Generar las ciudades aleatorias.
    // 2. Resolver el problema del agente viajero con algoritmo genético.
    // 3. Imprimir la mejor solución.

    int tamano_poblacion;
    int num_generaciones;
    int ciudades;

    cout << "Ingrese el tam de la poblacion: ";
    cin >> tamano_poblacion;
    cout << "Ingrese el numero de generaciones: ";
    cin >> num_generaciones;
    cout << "Ingrese el numero de ciudades: ";
    cin >> ciudades;

    resolver_problema_agente_viajero(generar_ciudades_aleatorias(ciudades), tamano_poblacion, num_generaciones);
    return 0;
}