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
enum AlocacaoEspectro { RD /*random*/, FF /*first-fit*/, MU /*most-used*/, FFO /*first-fit with optimized list*/};
enum TipoDeRede { Transparente, Translucida };
enum Respostas{ NAO, SIM };
enum Topologia { PacificBell , NSFNet, NFSNetMod, PontoaPonto4, PontoaPonto8, Top1, Top2, Top3 };
enum Simulacao { Sim_PbReq, Sim_OSNR, Sim_DAmp, Sim_NSlots, Sim_TreinoPSR, Sim_AlfaBetaOtimizado, Sim_Bigode };
enum SimOtimizacao { OtimizarAlfa, OtimizarBeta };
enum AlgoritmoRA{ NDF, CNF, TLP, SQP };
enum AlgoritmoRP{ FLR, FNS };
enum Dijkstra { MH /*considera custo fixo*/, CSP /*considera maneiras de alocar slots requisitados no enlace como custo*/, CSP_Acum /*considera número de nós até destino e maneiras de alocar slots requisitados no enlace como custo*/, SP /*menor caminho*/, DJK_SPeFormas /*mistura de SP com DJK_Formas*/, DJK_RuidoEFormas /*DJK considerando ruido normalizado do enlace e número de formas possíveis de alocação*/, LOR_NF /*LOR com Num Formas de Alocacao*/, LOR_A /*LOR com Dispobilidade*/, Dij_PSO /*PSO*/, OSNRR /*OSNR-R*/};

//Abre arquivos de texto para leitura ou escrita
class MAux {
	public:
		MAux();
		~MAux();
		static Def *Config;
		static ofstream Resul;
		static ofstream Metrica;
		static ofstream ResulOSNR;
		static ofstream Resul2;
		static ofstream ResulProbBloqTaxa;
		static ofstream ResulProbAceitTaxa;
		static ofstream ResulTaxaMedia;
		static ofstream ResultEsquemas;
		static ofstream ResulFFOmet_FFOext;
		static ofstream ResulFFOext_FFOmet;
		static ofstream ResulFFOconv_FFOext;
		static ofstream ResulFFOext_FFOconv;
		static ofstream ResultDAmpMaiorQueLimiar;
		static ofstream ResultDAmpMenorQueLimiar;
		static ofstream PSRLog;
		static ifstream Topol;
		static ifstream Topol2;
		static ifstream Topol3;
		static ifstream Topol4;
		static ifstream Topol5;
		static ifstream Topol6;
		static ifstream Topol7;
		static ifstream Topol8;


		//Variáveis necessárias em main_EONsimulator.cpp
		Event *firstEvent;
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
		static long double *Coeficientes;
		static long double *Topology;
		static long double *MinimasDistancias;

		static bool FlagRP_TLP; //Se estiver fazendo uma simulacao para o algoritmo TLP, flag marcada como true.
		int *RP_TLP_NodeUsage;
		static bool FlagRP_SQP; //Se estiver fazendo uma simulacao para o algoritmo SQP, flag marcada como true.
		int *RP_SQP_NodeUsage;
		static int NumRegeneradoresTotal;
		static int NumRegeneradoresPorNo;
		static int LNMax; //Parametro para o SQP

		TIME simTime;
		static vector<int> **FFlists;
		vector<Route*> *AllRoutes;
		static vector<Node> Rede;
		static vector<Enlace> *Caminho;
		static Respostas AvaliaOsnr;
		static Topologia escTop;
		static TipoDeRede escTipoRede;
		static Simulacao escSim;
		static SimOtimizacao escOtim;
		static AlgoritmoRA escRA;
		static AlgoritmoRP escRP;

};

#endif // MAIN_AUXILIAR_H
