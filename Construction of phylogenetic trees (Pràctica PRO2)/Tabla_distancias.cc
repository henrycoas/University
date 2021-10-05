#include "Tabla_distancias.hh"

//Constructora
Tabla_distancias::Tabla_distancias() {
	map<string, map<string, double>> tabla;
}

//Destructora
Tabla_distancias::~Tabla_distancias(){}

void Tabla_distancias::incrementar_tabla(string& id, map<string, double>& nuevas_dist) {
	tabla[id] = nuevas_dist;
	
	map<string, map<string, double> >::iterator it;
	map<string, double>::iterator ndist_it = nuevas_dist.begin();
	for(it = tabla.begin(); it != tabla.end(); ++it) {
		tabla[ndist_it->first][id] = ndist_it->second;
// 		it->second.insert(pair<string, double>(ndist_it->first, ndist_it->second));
		++ndist_it;
	}
}

void Tabla_distancias::imprimir_tabla() {
	map<string, map<string, double> >::const_iterator it;
	map<string, double>::const_iterator inner_it;
	for(it = tabla.begin(); it != tabla.end(); ++it) {
		string elem = it->first;
		cout << elem << ':';
		for(inner_it = ++tabla[elem].find(elem); inner_it != it->second.end(); ++inner_it) {
			cout << ' ' << inner_it->first << " (" << inner_it->second << ')';
		}
		cout << endl;
	}
}

void Tabla_distancias::disminuir_tabla(string& id) {
	tabla.erase(id);
	
	map<string, map<string, double> >::iterator it;
	for(it = tabla.begin(); it != tabla.end(); ++it) {
		it->second.erase(id);
	}
}

void Tabla_distancias::resetear_tabla() {
	tabla.clear();
}

double Tabla_distancias::consulta_distancia(string& id1, string& id2) {
	return tabla[id1][id2];
}

