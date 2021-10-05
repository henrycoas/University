/** @file Tabla_distancias.hh
    @brief Especificación de la clase Tabla_distancias 
*/
#ifndef TAB_DIST_HH
#define TAB_DIST_HH

#ifndef NO_DIAGRAM
#include <map>
#include <string>
#include <iostream>
#include <vector>
#endif
using namespace std;

class Tabla_distancias {

private:
	
	map<string, map<string, double>> tabla; //!<@brief Tabla de distancias entre elementos
	
public:
	
	/**@brief Constructora de la clase Tabla_distancias
   \pre cierto
   \post un map anidado en otro map
 */
	Tabla_distancias();
	
	/**@brief Destructora de la clase Tabla_distancias
 */
	~Tabla_distancias();

//Modificadoras
	/**@brief Elimina un identificador de la tabla
   \pre identificador del elemento a eliminar (string)
   \post se han borrado de la tabla todos los valores relacionados con el identificador dado
 */
	void disminuir_tabla(string& id);
	
	/**@brief Añade un elemento a la tabla y añade las nuevas distancias de este elemento a los ya presentes (y viceversa)
   \pre identificador string del elemento a añadir y map que contiene la distancia del nuevo elemento a todos los del conjunto de especies (él incluido)
   \post se ha añadido el elemento dado a la tabla y se han añadido las distancias que faltaban entre elementos
 */
	void incrementar_tabla(string& id, map<string, double>& nuevas_dist);
	
	/**@brief Reinicializa la tabla de modo que no contiene ningún elemento
   \pre cierto
   \post la tabla no contiene ningún elemento
 */
	void resetear_tabla();
	
//Consultora
	/**@brief Dados dos elementos, retorna la distancia entre ellos consultando la tabla del parámetro implícito
   \pre dos identificadores (strings)
   \post número decimal (double) que representa la distancia entre los elementos
 */
	double consulta_distancia(string& id1, string& id2);
	
	/**@brief Busca la menor distancia de la tabla y retorna los identificadores de los dos elementos que se encuentran a esa distancia
   \pre cierto
   \post par de identificadores de los elementos a menor distancia de la tabla
 */
	//Funcionalidad disponible en un futuro con la llegada de los clústers
	pair<string, string> busca_distancia_menor();
	
//Escritura
	/**@brief Imprime el "triángulo superior" de la tabla
   \pre cierto
   \post se ha escrito por el canal estándar de salida el triángulo de las distancias entre elementos
 */
	void imprimir_tabla();
};

#endif
