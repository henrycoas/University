/** @file Especie.hh
    @brief Especificación de la clase Especie 
*/
#ifndef ESP_HH
#define ESP_HH

#ifndef NO_DIAGRAM
#include <map>
#include <string>
#include <iostream>
#endif
using namespace std;

class Especie {
	
private:
	
	string gen; //!<@brief Secuencia de símbolos que representan el gen de una especie
	
	map<string, int> kmer; //!<@brief Diccionario con las secuencias kmer y su frecuencia en el gen de una especie
	
	static int k; //!<@brief Longitud deseada para las subsecuencias de los nucleótidos de los genes (variable global)
	
	/**@brief Crea, a partir del gen de la especie, un diccionario con sus subsecuencias kmer y sus respectivas frecuencias en el gen
   \pre cierto
   \post la especie contiene un map con sus subsecuencias kmer y su frecuencia en el gen
 */
	void crea_secuencia_kmer();
	
public:
	
	/**@brief Constructora de la clase Especie
   \pre cierto
   \post una especie vacía
 */
	Especie();
	
	/**@brief Destructora de la clase Especie
 */
	~Especie();

//Consultoras
	/**@brief Consulta el gen de la especie del parámetro implícito
   \pre cierto
   \post el resultado es el gen de la especie del parámetro implícito
 */
	string consultar_gen() const;
	
	/**@brief Retorna el diccionario de subsecuencias kmer de la especie del parámetro implícito
   \pre la especie existe
   \post map con las secuencias kmer y su frecuencia en el gen de la especie
 */
	map<string, int> consultar_kmer() const;

//Lectura
	/**@brief Hay preparado en el canal estándar de entrada un gen (string) del que se obtienen sus subsecuencias kmer
   \pre cierto
   \post el parámetro implícito contiene el gen y el map de subsecuencias
 */
	void lee_especie();

//Modificadora
	/**@brief Asigna un valor entero a la variable global k
   \pre un entero nk
   \post la variable global k ha tomado el valor introducido
 */
	static void asignacion_k(int& nk);
	
};

#endif
