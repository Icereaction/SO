simulador: config_read.o simulador.o
	gcc -g -o simulador.exe simulador_config_read.o simulador.o
	rm *.o
	
simulador_config_read.o: config_read.c
	gcc -g -c simulador_config_read.c
	
simulador.o: header.h simulador.c
	gcc -g -c header.h simulador.c
	
clean: 
	rm simulador.exe
	rm *.o
