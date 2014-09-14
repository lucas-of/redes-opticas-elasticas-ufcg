#ifndef Def_H
#define Def_H

#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

typedef long double TIME;

#include "General.h"

class Def {
    private:
        static vector<int> GrauNo; //O grau de cada no
        static vector<double> LaNet; //La é o tráfego por cada slot
        static int Nnodes; // N é o número de nós;
        static long double numReqMax; //é o número máximo de requisições
        static int SE; //S é o número de slots por Enlace
        static int SR; //SR é o número máximo de slots requisitados

    public:
        static double MAX_DOUBLE; //limites - maior DOUBLE possível
        static int MAX_INT; //limites - maior INT possível
        static double MAX_LONGDOUBLE;  //limites - maior LONG DOUBLE possível
        static long double netOccupancy; //ocupação da rede
        static long double numHopsPerRoute; //número de nós por rota
        static long double numReq; //número de requisições
        static long double numReq_Bloq; //número de requisições bloqueadas
        static long double numSlots_Bloq; //número de slots bloqueados
        static long double numSlots_Req; //número de slots requisitados

        static void clearGrauNo();
        static int getGrauNo(int);
        static double getLaNet(int);
        static int getNnodes();
        static long double getNumReqMax();
        static void setNnodes(int);
        static int getSE();
        static int getSR();
        static void setGrauNo(int Grau);
        static void setLaCheck(double); //confirma que a soma do tráfego é normalizado
        static void setLaRandom(double); //tráfego aleatório entre slots
        static void setLaUniform(double); //tráfego uniforme entre enlaces
        static void setNumReqMax(long double);
        static void setSE(int);
        static void setSR(int);
};
#endif
