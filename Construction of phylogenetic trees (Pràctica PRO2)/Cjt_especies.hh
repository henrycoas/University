/** @file Cjt_especies.hh
    @brief Especificación de la clase Cjt_especies 
*/

#ifndef CJT_ESP_HH
#define CJT_ESP_HH
#include "Especie.hh"
#include "Tabla_distancias.hh"
using namespace std;

class Cjt_especies {
	
private:
	
	map<string, Especie> especies; //!<@brief Conjunto de Especies indexadas por su identificador
	
	Tabla_distancias interespecies; //!<@brief Tabla de distancias entre especies
	
	/**@brief Retorna un par de números en que el primero es el resultado de la intersección de dos maps de subsecuencias kmer y el segundo es el resultado de la reunión de los mismos maps
   \pre dos maps con la informacion sobre cada una de las subsecuencias de un gen y sus respectivas frecuencias
   \post par de números en formato double (por comodidad) donde el primero es la interseccion de las subsecuencias y el segundo, la reunion de las subsecuencias
 */
	//Apoyo de calcula_distancia 
	pair<double, double> interseccion_y_reunion(map<string, int>& kmer1, map<string, int>& kmer2) const;
	
	/**@brief Actualiza la tabla de distancias debido a que se ha añadido o eliminado una especie del conjunto
   \pre identificador de la especie que ha sufrido una modificación y booleano que indica si la especie debe ser añadida (booleano a cierto) o eliminada (booleano a falso)
   \post la tabla de distancias contiene exclusivamente las especies del conjunto
 */
	//Apoyo de crea_especie y elimina_especie
	void actualiza_tabla_especies(string& id_especie, const bool& incrementar);
	
	/**@brief Calcula la distancia entre dos especies
   \pre dos identificadores (string)
   \post número en formato double que representa la distancia entre dos especies (igual en ambos sentidos)
 */
	//Apoyo a actualiza_tabla_especies
	double calcula_distancia(string& id1, string& id2);
	
public:
	
	/**@brief Constructora de la clase Cjt_especies
   \pre cierto
   \post un conjunto vacío de especies
 */
	Cjt_especies();
	
	/**@brief Destructora de la clase Cjt_especies
 */
	~Cjt_especies();

//Consultoras
	/**@brief Retorna un booleano que indica si la especie está en el conjunto o no
   \pre un identificador (string)
   \post booleano a cierto si el conjunto contiene la especie; a falso en caso contrario
 */
	//Apoyo general
	bool existe_id(const string& id_especie) const;
	
	/**@brief Imprime una indicación de si una especie existe o no
   \pre un identificador (string)
   \post se ha escrito por el canal estándar de salida: "SI", si el conjunto de especies contiene la especie; o "NO", en caso contrario 
 */
	//5
	void existe_especie(const string& id_especie) const;

	/**@brief Imprime el gen asociado a la especie
   \pre un identificador (string)
   \post se ha escrito por el canal estándar de salida el gen que pertenece al identificador dado 
 */
	//2
	void obtener_gen(const string& id_especie);
	
	/**@brief Imprime la distancia entre dos especies
   \pre dos identificadores (strings)
   \post se ha escrito por el canal estándar de salida la distancia entre las dos especies
 */
	//3
	void distancia(string& id_especie1, string& id_especie2);
	
//Creación y eliminación de especies
	/**@brief Crea una especie y la añade al conjunto
   \pre identificador del gen (string) y Especie
   \post la Especie introducida forma parte del conjunto de especies (y está indexada por el identificador dado)
 */
	//1
	void crea_especie(string& id_especie, const Especie& sp);
	
	/**@brief Elimina una especie del conjunto
   \pre un identificador de especie (string)
   \post se ha eliminado la especie del conjunto y de la tabla de distancias
 */
	//4
	void elimina_especie(string& id_especie);
	
//Lectura y escriptura
	/**@brief Se descarta el conjunto de especies anterior y se lee uno nuevo
   \pre el canal estándar de entrada contiene un entero positivo n y una secuencia de n especies (pares identificador-gen) con identificadores distintos entre sí
   \post el parámetro implícito contiene el conjunto de especies leídas y las distancias entre ellas se han añadido a la tabla
 */
	//6
	void lee_cjt_especies();
	
	/**@brief Imprime el conjunto de especies
   \pre cierto
   \post se han escrito por el canal estándar de salida el identificador y el gen de las especies del conjunto que contiene el parámetro implícito
 */
	//7
	void imprime_cjt_especies() const;
	
	/**@brief Imprime la tabla de distancias entre cada par de especies del conjunto de especies
   \pre cierto
   \post se ha escrito por el canal estándar de salida las distancias entre cada especie
 */
	//8
	void tabla_distancias();
	
	};
	
#endif
