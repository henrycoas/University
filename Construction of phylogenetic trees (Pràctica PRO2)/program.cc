/** @file main.cc
 * @brief Programa principal para la creación de un árbol filogenético
 */
#include "Cjt_especies.hh"
using namespace std;

/**@brief Programa principal
 */
int main() {
	Cjt_especies a;
// 	Cjt_clusters clt;
	
	int nk;
	cin >> nk;
	Especie sk;		//La especie sk es usada para hacer la declaracion global de la variable k
	sk.asignacion_k(nk);
	
	string op;
	while (cin >> op and op != "fin") {
		if (op == "crea_especie") {
			string id;
			cin >> id;
			cout << "# crea_especie " << id << ' ';
			if (not a.existe_id(id)) {
				Especie sp;
				sp.lee_especie();
				a.crea_especie(id, sp);
				cout << sp.consultar_gen() << endl;
			} else {
				string gen_desechable;
				cin >> gen_desechable;
				cout << gen_desechable << endl;
				cout << "ERROR: La especie " << id << " ya existe." << endl;
			}
		} 
		else if (op == "obtener_gen") {
			string id;
			cin >> id;
			cout << "# obtener_gen " << id << endl;
			if (a.existe_id(id)) {
				a.obtener_gen(id);
			} else {
				cout << "ERROR: La especie " << id << " no existe." << endl;
			}
		} 
		else if (op == "distancia") {
			string id1, id2;
			cin >> id1 >> id2;
			cout << "# distancia " << id1 << ' ' << id2 << endl;
			bool existe_id1 = a.existe_id(id1);
			bool existe_id2 = a.existe_id(id2);
			if (existe_id1 and existe_id2) {
				a.distancia(id1, id2);
			} else if (not existe_id1 and not existe_id2) {
				cout << "ERROR: La especie " << id1 << " y la especie " << id2 << " no existen." << endl;
			} else if (not existe_id1) {
				cout << "ERROR: La especie " << id1 << " no existe." << endl;
			} else {
				cout << "ERROR: La especie " << id2 << " no existe." << endl;
			}
		} 
		else if (op == "elimina_especie") {
			string id;
			cin >> id;
			cout << "# elimina_especie " << id << endl;
			if (a.existe_id(id)) {
				a.elimina_especie(id);
			} else {
				cout << "ERROR: La especie " << id << " no existe." << endl;
			}
		} 
		else if (op == "existe_especie") {
			string id;
			cin >> id;
			cout << "# existe_especie " << id << endl;
			a.existe_especie(id);
		} 
		else if (op == "lee_cjt_especies") {
			cout << "# lee_cjt_especies" << endl;
			a.lee_cjt_especies();
		} 
		else if (op == "imprime_cjt_especies") {
			cout << "# imprime_cjt_especies" << endl;
			a.imprime_cjt_especies();
		} 
		else if (op == "tabla_distancias") {
			cout << "# tabla_distancias" << endl;
			a.tabla_distancias();
		} 
		/*
		else if (op == 9) {
			clt.inicializa_clusters();
		}
		else if (op == 10) {
			if (clt.numero_de_clusters() > 1) {
				clt.ejecuta_paso_wpgma();
			} else {
				cout << "No se puede ejecutar un paso del algoritmo WPGMA: el numero de clusters del conjunto no es mayor que uno" << endl;
			}
		}
		else if (op == 11) {
			string id;
			cin >> id;
			if (clt.existe_cluster(id)) {
				clt.imprime_cluster(id);
			} else {
				cout << "Impresion fallida: no existe un cluster con la id dada" << endl;
			}	
		}
		else if (op == 12) {
			clt.imprime_arbol_filogenetico();
		}
		*/
		cout << endl;
	}
}




/*! \mainpage Creación de un árbol filogenético: Documentación
 *
 * \section intro_sec Introducción
 *
 * Esta práctica trata de construir el árbol filogenético para un conjunto de N especies utilizando el método conocido como WPGMA (weighted pair group with arithmetic mean).
 * 
 * Para ello, he decidido implementar cuatro nuevas clases: Especie, Cjt_especies, Cjt_clusters y Tabla_distancias.
 */
