#include "Main_Auxiliar.h"

Def* MAux::Config = new Def(NULL);
ofstream MAux::Resul("Result.txt");
ofstream MAux::Metrica("Metrica.txt");
ofstream MAux::ResulOSNR("ResultOSNR.txt");
ofstream MAux::Resul2("Result2.txt");
ofstream MAux::ResulProbBloqTaxa("ProbBloqTaxa.txt");
ofstream MAux::ResulProbAceitTaxa("ProbAceitTaxa.txt");
ofstream MAux::ResulTaxaMedia("TaxaMedia.txt");
ofstream MAux::ResulFFOmet_FFOext("ResultFFOmet_FFOext.txt");
ofstream MAux::ResulFFOext_FFOmet("ResultFFOext_FFOmet.txt");
ofstream MAux::ResulFFOconv_FFOext("ResultFFOconv_FFOext.txt");
ofstream MAux::ResulFFOext_FFOconv("ResultFFOext_FFOconv.txt");
ofstream MAux::ResultDAmpMaiorQueLimiar("DAmpMaiorQueLimiar.txt");
ofstream MAux::ResultDAmpMenorQueLimiar("DAmpMenorQueLimiar.txt");
ofstream MAux::ResultEsquemas("ResultEsquemas.txt");
ofstream MAux::PSRLog("PSRLog.txt");
ifstream MAux::Topol("TopologyNFS.txt");
ifstream MAux::Topol2("TopologyPBell.txt");
ifstream MAux::Topol3("TopologyRed8.txt");
ifstream MAux::Topol4("TopologyRed4.txt");
ifstream MAux::Topol5("TopologyNFSMod.txt");
ifstream MAux::Topol6("TopologyTop1.txt");
ifstream MAux::Topol7("TopologyTop2.txt");
ifstream MAux::Topol8("TopologyTop3.txt");

bool MAux::ExpComp;
int MAux::Alg_Aloc;
int MAux::Alg_Routing;
int MAux::Br;
long double MAux::laE;
long double MAux::laNet;
long double MAux::OSNRMin;
long double MAux::OSNRMax;
long double MAux::OSNRPasso;
long double MAux::LaNetMin;
long double MAux::LaNetMax;
long double MAux::LaPasso;
long double MAux::DAmpMin;
long double MAux::DAmpMax;
long double MAux::DAmpPasso;
long double MAux::mu;
long double MAux::muC;
long double *MAux::Topology;
long double *MAux::Coeficientes;
long double *MAux::MinimasDistancias;
vector<int> **MAux::FFlists = NULL;
vector<Node> MAux::Rede;
vector<Enlace> *MAux::Caminho;
Respostas MAux::AvaliaOsnr;
Topologia MAux::escTop;
Simulacao MAux::escSim;
SimOtimizacao MAux::escOtim;
TipoDeRede MAux::escTipoRede;

MAux::MAux(){
	AllRoutes = new vector<Route*>[Def::getNnodes()*Def::getNnodes()];
	for (int i = 0; i < Def::Nnodes*Def::Nnodes; i++)
		while (!AllRoutes[i].empty()) {
			delete AllRoutes[i].back();
			AllRoutes[i].pop_back();
		}
	firstEvent = NULL;
}

MAux::~MAux() {
	delete[] AllRoutes;
}
