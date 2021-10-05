OPCIONS = -D_JUDGE_ -D_GLIBCXX_DEBUG -O2 -Wall -Wextra -Werror -Wno-sign-compare -std=c++11

program.exe: Especie.o Cjt_especies.o Tabla_distancias.o program.o
	g++ -o program.exe Especie.o Cjt_especies.o Tabla_distancias.o program.o
	
program.o: program.cc
	g++ -c program.cc $(OPCIONS)
	
Especie.o: Especie.cc Especie.hh
	g++ -c Especie.cc $(OPCIONS)
	
Cjt_especies.o: Cjt_especies.cc Cjt_especies.hh Tabla_distancias.cc Tabla_distancias.hh Especie.cc Especie.hh
	g++ -c Cjt_especies.cc $(OPCIONS)

Tabla_distancias.o: Tabla_distancias.cc Tabla_distancias.hh
	g++ -c Tabla_distancias.cc $(OPCIONS)

practica.tar: Cjt_especies.cc Cjt_especies.hh Tabla_distancias.cc Tabla_distancias.hh Especie.cc Especie.hh program.cc Makefile
	tar -cvf practica.tar Cjt_especies.cc Cjt_especies.hh Tabla_distancias.cc Tabla_distancias.hh Especie.cc Especie.hh program.cc Makefile

clean:
	del -f *.o *.exe
