#include "Cjt_especies.hh"

//Constructora
Cjt_especies::Cjt_especies() {
	map<string,Especie> especies;
	Tabla_distancias interespecies;
}

//Destructora
Cjt_especies::~Cjt_especies(){}

//Apoyo de crea_especie y elimina_especie
void Cjt_especies::actualiza_tabla_especies(string& id_especie, const bool& incrementar) {
	if (incrementar) {
		map<string, double> nuevas_dist;
		map<string, Especie>::const_iterator it_sp;
		//harรก falta calcular las distancias de la nueva id a las ya presentes
		for(it_sp = especies.begin(); it_sp != especies.end(); ++it_sp) {
			string id_destino = it_sp->first;
			if (id_destino != id_especie) {
				nuevas_dist[id_destino] = calcula_distancia(id_especie, id_destino);
			} else {
				nuevas_dist[id_destino] = 0;
			}	
			
		}
		interespecies.incrementar_tabla(id_especie, nuevas_dist);
	} else {
		interespecies.disminuir_tabla(id_especie);
	}
}

//Apoyo general 
bool Cjt_especies::existe_id(const string& id_especie) const {
	map<string, Especie>::const_iterator it = especies.find(id_especie);
	return it != especies.end();
}

//5
void Cjt_especies::existe_especie(const string& id_especie) const {
	map<string, Especie>::const_iterator it = especies.find(id_especie);
	if (it != especies.end()) {
		cout << "SI" << endl;
	} else {
		cout << "NO" << endl;
	}
}

//1
void Cjt_especies::crea_especie(string& id_especie, const Especie& sp) {
	especies[id_especie] = sp;
	actualiza_tabla_especies(id_especie, true);
}

//2
void Cjt_especies::obtener_gen(const string& id_especie) {
	cout << especies[id_especie].consultar_gen() << endl;
}

//Apoyo a calcula_distancia
pair<double, double> Cjt_especies::interseccion_y_reunion(map<string, int>& kmer1, map<string, int>& kmer2) const {
	map<string, int>::iterator it1 = kmer1.begin();
	map<string, int>::iterator it2 = kmer2.begin();
	double interseccion = 0, reunion = 0;
	while (it1 != kmer1.end() and it2 != kmer2.end()) {
		string seq1 = it1->first;
		string seq2 = it2->first;
		if (seq1 == seq2) {
			interseccion += min(kmer1[seq1], kmer2[seq2]);
			reunion += max(kmer1[seq1], kmer2[seq2]);
			++it1; 
			++it2;
		} else if (seq1 < seq2) {
			reunion += kmer1[seq1];
			++it1;
		} else {
			reunion += kmer2[seq2];
			++it2;
		}
	}
	while (it1 != kmer1.end()) {
		reunion += kmer1[it1->first];
		++it1;
	}	
	while (it2 != kmer2.end()) {
		reunion += kmer2[it2->first];
		++it2;
	}
	return make_pair(interseccion, reunion);
}

//Apoyo a actualiza_tabla_especies
double Cjt_especies::calcula_distancia(string& id1, string& id2) {
	map<string, int> kmer1 = especies[id1].consultar_kmer();
	map<string, int> kmer2 = especies[id2].consultar_kmer();
	
	pair<double, double> iyr = interseccion_y_reunion(kmer1, kmer2);
	
	double result = (1 - (iyr.first / iyr.second))*100;
	
	return result;
}

//3
void Cjt_especies::distancia(string& id1, string& id2) {
// 	cout.setf(ios::fixed);
// 	cout.precision(4);
	cout << interespecies.consulta_distancia(id1, id2) << endl;
}

//4
void Cjt_especies::elimina_especie(string& id_especie) {
	especies.erase(id_especie);
 	actualiza_tabla_especies(id_especie, false);
}

//6
void Cjt_especies::lee_cjt_especies() {
	//Se descartan las especies del cjt previo y se borran todas de la tabla
	especies.clear();
	interespecies.resetear_tabla();
	
	int n_especies;
	cin >> n_especies;
	string id_especie;
	for(int i = 0; i < n_especies; ++i) {
		cin >> id_especie;
		Especie sp;
		sp.lee_especie();
		crea_especie(id_especie, sp);
	}
}

//7
void Cjt_especies::imprime_cjt_especies() const {
	for(map<string, Especie>::const_iterator it = especies.begin(); it != especies.end(); ++it) {
		cout << it->first << ' ' << it->second.consultar_gen() << endl;
	}
}

//8
void Cjt_especies::tabla_distancias() {
	interespecies.imprimir_tabla();
}
