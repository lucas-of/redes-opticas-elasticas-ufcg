#include "Regeneradores.h"
#include "Def.h"
#include "Main_Auxiliar.h"
#include "RWA.h"

long double Simula_Rede(Def *Config, MAux *Aux);

Regeneradores::Regeneradores() {};

void Regeneradores::RP_NDF(int NumTotalRegeneradores, int NumRegeneradoresPorNo) {
	assert (NumTotalRegeneradores % NumRegeneradoresPorNo == 0);
	int NumNoTransparentes = NumTotalRegeneradores/NumRegeneradoresPorNo;
	assert(NumNoTransparentes <= Def::Nnodes);

	vector<int> GrauNo = Def::GrauNo;
	for (int i = 0; i < NumNoTransparentes; i++) {
		int MaiorGrau = GrauNo.at(0);
		for (int j = 0; j < GrauNo.size(); j++)
			if (MaiorGrau < GrauNo.at(j)) MaiorGrau = GrauNo.at(j);

		vector<int> NosMaximos;
		for (int j = 0; j < GrauNo.size(); j++)
			if (GrauNo.at(j) == MaiorGrau) NosMaximos.push_back(j);

		int NoEscolhido = ceil( General::uniforme(0, NosMaximos.size()) );
		MAux::Rede.at(NoEscolhido).set_TipoNo( Node::Translucido );
		MAux::Rede.at(NoEscolhido).set_NumRegeneradores( NumRegeneradoresPorNo );

		GrauNo.at(NoEscolhido) = -1;
	}
}

void Regeneradores::RP_CNF(int NumTotalRegeneradores, int NumRegeneradoresPorNo) {
	assert (NumTotalRegeneradores % NumRegeneradoresPorNo == 0);
	int NumNoTransparentes = NumTotalRegeneradores/NumRegeneradoresPorNo;
	assert(NumNoTransparentes <= Def::Nnodes);

	vector <int> CentralidadeNo(Def::Nnodes);
	for (int i = 0; i < CentralidadeNo.size(); i++) CentralidadeNo.at(i) = 0;

	MAux MainAux;
	RWA::DijkstraSP(&MainAux);

	for (int orN = 0; orN < Def::getNnodes(); orN++) {
		for (int deN = 0; deN < Def::getNnodes(); deN++) {
			if (orN == deN) continue;
			for (int No = 0; No <= MainAux.AllRoutes[orN*Def::Nnodes + deN].at(0)->getNhops(); No++)
				CentralidadeNo.at(MainAux.AllRoutes[orN*Def::Nnodes + deN].at(0)->getNode(No))++;
		}
	}

	for (int i = 0; i < NumNoTransparentes; i++) {
		int MaiorGrau = CentralidadeNo.at(0);
		for (int j = 0; j < CentralidadeNo.size(); j++)
			if (MaiorGrau < CentralidadeNo.at(j)) MaiorGrau = CentralidadeNo.at(j);

		vector<int> NosMaximos;
		for (int j = 0; j < CentralidadeNo.size(); j++)
			if (CentralidadeNo.at(j) == MaiorGrau) NosMaximos.push_back(j);

		int NoEscolhido = ceil( General::uniforme(0, NosMaximos.size()) );
		MAux::Rede.at(NoEscolhido).set_TipoNo( Node::Translucido );
		MAux::Rede.at(NoEscolhido).set_NumRegeneradores( NumRegeneradoresPorNo );
		CentralidadeNo.at(NoEscolhido) = -1;
	}
}

void Regeneradores::RP_TLP(int NumTotalRegeneradores, int NumRegeneradoresPorNo) {
	assert (NumTotalRegeneradores % NumRegeneradoresPorNo == 0);
	int NumNoTransparentes = NumTotalRegeneradores/NumRegeneradoresPorNo;
	assert(NumNoTransparentes <= Def::Nnodes);

	MAux::FlagRP_TLP = true;
	MAux MainAux;
	Def Config(NULL);
	Simula_Rede(&Config, &MainAux);

	for (int i = 0; i < NumNoTransparentes; i++) {
		int MaiorGrau = MainAux.RP_TLP_NodeUsage[0];
		for (int j = 0; j < Def::Nnodes; j++)
			if (MaiorGrau < MainAux.RP_TLP_NodeUsage[j]) MaiorGrau = MainAux.RP_TLP_NodeUsage[j];

		vector<int> NosMaximos;
		for (int j = 0; j < Def::Nnodes; j++)
			if (MainAux.RP_TLP_NodeUsage[j] == MaiorGrau) NosMaximos.push_back(j);

		int NoEscolhido = ceil( General::uniforme(0, NosMaximos.size()) );
		MAux::Rede.at(NoEscolhido).set_TipoNo( Node::Translucido );
		MAux::Rede.at(NoEscolhido).set_NumRegeneradores( NumRegeneradoresPorNo );
		MainAux.RP_TLP_NodeUsage[NoEscolhido] = -1;
	}

	MAux::FlagRP_TLP = false;
}