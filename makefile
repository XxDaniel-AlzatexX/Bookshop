all: ps rp

ps: ps.o BaseDatos.h
	gcc -o sp sp.o

ps.o: ps.c BaseDatos.h
	gcc -c sp.c

rp: rp.o BaseDatos.h
	gcc -o rp rp.o

rp.o: rp.c BaseDatos.h
	gcc -c rp.c
