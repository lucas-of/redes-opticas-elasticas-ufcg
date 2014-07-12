#ifndef Def_H
#define Def_H

#include <assert.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
using namespace std;
typedef long double TIME;

#include "General.h"

////////////////////////////////////////////////////////////////
class Def
{
public:
    static void setNnodes(int);
    static int getNnodes();
    static void setSR(int);
    static int getSR();
    static void setSE(int);
    static int getSE();
    static void setNumReqMax(long double);
    static long double getNumReqMax();
    static void setLaUniform(double);
    static void setLaRandom(double);
    static void setLaManual(double);
    static void setLaCheck(double);
    static double getLaNet(int);
private:
    static int Nnodes; // N é o número de nós;
    static int SR; //SR é o número máximo de slots requisitados
    static int SE; //S é o número de slots por Enlace
    static vector<double> LaNet;
    static long double numReqMax;
public:
    static long double numReq;
    static long double numReq_Bloq;
    static long double numSlots_Bloq;
    static long double numSlots_Req;
    static long double numHopsPerRoute;
    static long double netOccupancy;

    static int MAX_INT;
    static double MAX_DOUBLE;
    static double MAX_LONGDOUBLE;
};

#endif
