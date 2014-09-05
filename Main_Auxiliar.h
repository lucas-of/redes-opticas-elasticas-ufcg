#ifndef MAIN_AUXILIAR_H
#define MAIN_AUXILIAR_H

#include <iostream>
#include <fstream>
#include <vector>
#include "Event.h"

using namespace std;

//Abre arquivos de texto para leitura ou escrita
ofstream Resul("Result.txt");
ofstream Metrica("Metrica.txt");
ofstream Resul2("Result2.txt");
ofstream ResulFFOmet_FFOext("ResultFFOmet_FFOext.txt");
ofstream ResulFFOext_FFOmet("ResultFFOext_FFOmet.txt");
ofstream ResulFFOconv_FFOext("ResultFFOconv_FFOext.txt");
ofstream ResulFFOext_FFOconv("ResultFFOext_FFOconv.txt");
ifstream Topol("Topology.txt");

//Definição de variáveis que virão a ser usadas em main_EONsimulator.cpp
enum Dijkstra { DJK /*considera custo fixo*/, DJK_Formas /*considera maneiras de alocar slots requisitados no enlace como custo*/, DJK_Acum /*considera número de nós até destino e maneiras de alocar slots requisitados no enlace como custo*/};
enum AlocacaoEspectro { RD /*random*/, FF /*first-fit*/, MU /*most-used*/, FFO /*first-fit with optimized list*/};

//Variáveis necessárias em main_EONsimulator.cpp
bool ExpComp;
bool ***Topology_S;
bool *Traf;
Event *firstEvent;
int Alg_Aloc;
int Alg_Routing;
long double laE;
long double laNet;
long double LaNetMin;
long double LaNetMax;
long double LaPasso;
long double mu;
long double muC;
long double **Topology;
TIME simTime;
vector<int> **FFlists = NULL;
vector<int> **RouteInt;
vector<Route*> *AllRoutes;

#endif // MAIN_AUXILIAR_H
