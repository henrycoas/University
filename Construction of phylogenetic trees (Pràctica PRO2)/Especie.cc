#include "Especie.hh"

Especie::Especie() {
	string gen;
	map<string, int> kmer;
}

Especie::~Especie(){}

int Especie::k;

//Después de introducir el gen de una especie que no existe en el conjunto, se crea su secuencia kmer
void Especie::crea_secuencia_kmer() {
	map<string, int>::const_iterator it_check;
	int it_aux = 0;
	for(string::iterator it = gen.begin(); it < gen.end() - k + 1; ++it) {
		string kseq = gen.substr(it_aux, k);
		it_check = kmer.find(kseq);
		if (it_check == kmer.end()) {
			kmer.insert(make_pair(kseq, 1));
		} else {
			++kmer[kseq];
		}
		++it_aux;
	}
}

string Especie::consultar_gen() const {
	return gen;
}

map<string, int> Especie::consultar_kmer() const{
	return kmer;
}

void Especie::lee_especie() {
	cin >> gen;
	crea_secuencia_kmer();
}

void Especie::asignacion_k(int& nk) {
	k = nk;
}
