#ifndef MAIN_AUXILIAR_H
#define MAIN_AUXILIAR_H

#include <iostream>
#include <fstream>
#include <vector>
#include "Event.h"
#include "Node.h"
#include "Enlace.h"
#include "Conexao.h"
#include "Constantes.h"

//Definição de variáveis que virão a ser usadas em main_EONsimulator.cpp
enum Dijkstra { DJK /*considera custo fixo*/, DJK_Formas /*considera maneiras de alocar slots requisitados no enlace como custo*/, DJK_Acum /*considera número de nós até destino e maneiras de alocar slots requisitados no enlace como custo*/, SP /*menor caminho*/, DJK_SPeFormas /*mistura de SP com DJK_Formas*/};
enum AlocacaoEspectro { RD /*random*/, FF /*first-fit*/, MU /*most-used*/, FFO /*first-fit with optimized list*/};
enum Respostas{ NAO, SIM };
enum Topologia { PacificBell , NSFNet, NFSNetMod, PontoaPonto4, PontoaPonto8, Top1, Top2 };
enum Simulacao { Sim_PbReq, Sim_OSNR, Sim_DAmp, Sim_NSlots };

//Abre arquivos de texto para leitura ou escrita
class MAux {
	public:
		static ofstream Resul;
		static ofstream Metrica;
		static ofstream ResulOSNR;
		static ofstream Resul2;
		static ofstream ResulProbBloqTaxa;
		static ofstream ResulProbAceitTaxa;
		static ofstream ResulTaxaMedia;
		static ofstream ResulFFOmet_FFOext;
		static ofstream ResulFFOext_FFOmet;
		static ofstream ResulFFOconv_FFOext;
		static ofstream ResulFFOext_FFOconv;
		static ofstream ResultDAmpMaiorQueLimiar;
		static ofstream ResultDAmpMenorQueLimiar;
		static ifstream Topol;
		static ifstream Topol2;
		static ifstream Topol3;
		static ifstream Topol4;
		static ifstream Topol5;
		static ifstream Topol6;
		static ifstream Topol7;


		//Variáveis necessárias em main_EONsimulator.cpp
		static bool ExpComp;
		static bool ***Topology_S;
		static bool *Traf;
		static Event *firstEvent;
		static int Alg_Aloc;
		static int Alg_Routing;
		static long double laE;
		static long double laNet;
		static long double OSNRMin;
		static long double OSNRMax;
		static long double OSNRPasso;
		static long double LaNetMin;
		static long double LaNetMax;
		static long double LaPasso;
		static long double DAmpMin;
		static long double DAmpMax;
		static long double DAmpPasso;
		static long double mu;
		static long double muC;
		static long double **Topology;
		static long double **MinimasDistancias;
		static TIME simTime;
		static vector<int> **FFlists;
		static vector<Route*> *AllRoutes;
		static vector<Node> Rede;
		static vector<Enlace> *Caminho;
		static Respostas AvaliaOsnr;
		static Topologia escTop;
		static Simulacao escSim;
};

#endif // MAIN_AUXILIAR_H
