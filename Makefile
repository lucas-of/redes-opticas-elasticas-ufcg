CC = g++
CFLAGS = -g -O -c

all: Conexao Route Heuristics Def General Main
	$(CC) -o EON_Simulator Conexao.o Route.o Heuristics.o Def.o General.o main_EONsimulator.o

Conexao:
	$(CC) $(CFLAGS) Conexao.cpp

Route:
	$(CC) $(CFLAGS) Route.cpp

Heuristics:
	$(CC) $(CFLAGS) Heuristics.cpp

Def:
	$(CC) $(CFLAGS) Def.cpp

General:
	$(CC) $(CFLAGS) General.cpp

Main:
	$(CC) $(CFLAGS) main_EONsimulator.cpp

#incluir a linha #include <limits.h> em Heuristics.h
#comentar a linha #include "Path.h" em main
