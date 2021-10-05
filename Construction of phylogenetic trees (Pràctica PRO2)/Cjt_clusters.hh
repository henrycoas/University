/** @file Cjt_Clusters.hh
    @brief Especificación de la clase Cjt_clusters 
*/
#ifndef CJT_CLUST_HH
#define CJT_CLUST_HH

#include "Cjt_especies.hh"
#include "BinTree.hh"
using namespace std;

class Cjt_clusters {
	
private:
	
	BinTree<pair<string, double>> arbol; //!<@brief Árbol filogenético en el que cada nodo contiene un identificador y la distancia a sus nodos hijos
	
	Tabla_distancias interclusters; //!<@brief Tabla de las distancias entre clústers
	
	/**@brief Devuelve un map con las distancias de un cluster dado a todos los clústeres de la tabla de distancias actual
   \pre un identificador de clúster (string)
   \post el resultado es un map donde el identificador es el clúster destino y contiene la distancia desde el clúster origen dado
 */
	map<string, double> distancia_clusters(const string& id_cluster);
	
	/**@brief Actualiza la tabla de las distancias entre clústers después de que dos clústers hayan sido fusionados
   \pre dos identificadores de los clústeres que se han fusionado (strings)
   \post se han borrado las filas y columnas de id_cluster1 e id_cluster2 y se ha añadido la nueva fila y columna del resultado de su fusión. Además, se han actualizado las distancias entre clústers de la tabla
 */
	//Apoyo 9 y 10
	void actualiza_tabla_clusters(const string& id_cluster1, const string& id_cluster2);
	
public:
	
	/**@brief Constructora de la clase Cjt_clusters
   \pre cierto
   \post un conjunto vacío de clústers
 */
	Cjt_clusters();
	
	/**@brief Destructora de la clase Cjt_clusters
 */
	~Cjt_clusters();

//Consultora
	/**@brief Retorna un booleano que indica si el identificador coincide con el de algún clúster del conjunto del parámetro implícito
   \pre un identificador de clúster (string)
   \post cierto si el clúster con el identificador id dado existe; falso en caso contrario
 */
	bool existe_cluster(const string& id_cluster) const;
	
	/**@brief Retorna la cantidad de clústers que contiene el conjunto
   \pre cierto
   \post el resultado es un entero que equivale al numero de clústers del conjunto
 */
	int numero_de_clusters() const;
	
//Modificadoras
	/**@brief Inicializa el conjunto de clústers con el cjt de especies actual e imprime los clústeres y la tabla de distancias entre clústers
   \pre cierto
   \post el cjt de clusters contiene el cjt de especies y se ha escrito por el canal estándar de salida los clústeres y la tabla de distancias interclústers
 */
	//9
	void inicializa_clusters();
	
	/**@brief Ejecuta un paso del algoritmo WPGMA e imprime la tabla de distancias actualizada
   \pre cierto
   \post se han fusionado los dos clústers a menor distancia en uno nuevo y se ha actualizado e imprimido la tabla de distancias entre clústers
 */
	//10
	void ejecuta_paso_wpgma();
	
//Impresión
	/**@brief Dado un identificador, imprime el clúster (árbol) con dicho identificador
   \pre un identificador (string)
   \post se ha escrito por el canal estándar de salida la estructura arborescente del clúster
 */
	//11
	void imprime_cluster(const string& id_cluster); const
	
	/**@brief Se reinicializa el cjt de clústers con el cjt de especies, se aplica el algoritmo WPGMA y se imprimen el árbol filogenético y la distancia entre cada clúster y sus hojas descendientes
   \pre cierto
   \post se ha ha escrito por el canal estándar de salida el árbol filogenético actualizado y la distancia entre cada clúster y sus hojas descendientes
 */
	//12
	void imprime_arbol_filogenetico();
};

#endif
