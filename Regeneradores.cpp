#include "Regeneradores.h"
#include "Def.h"
#include "Main_Auxiliar.h"
#include "RWA.h"
#include "Event.h"

long double AvaliarOSNR(const Route *Rota, Def *Config);
long double Simula_Rede(Def *Config, MAux *Aux);
int SlotsReq(int Ran, EsquemaDeModulacao Esquema);
void TryToConnect(const Route* route, const int NslotsReq, int& NslotsUsed, int& si, Def *Config);

int Regeneradores::SQP_LNMax;
long double Regeneradores::BR = 100E9;

Regeneradores::Regeneradores() {};

void Regeneradores::RP_NDF(int NumTotalRegeneradores, int NumRegeneradoresPorNo) {
	assert (NumTotalRegeneradores % NumRegeneradoresPorNo == 0);
	int NumNoTransparentes = NumTotalRegeneradores/NumRegeneradoresPorNo;
	assert(NumNoTransparentes <= Def::Nnodes);

	vector<int> GrauNo;
	for (int i = 0; i < Def::GrauNo.size(); i++)
		GrauNo.push_back( Def::GrauNo.at(i) );
	for (int i = 0; i < NumNoTransparentes; i++) {
		int MaiorGrau = GrauNo.at(0);
		for (int j = 0; j < GrauNo.size(); j++)
			if (MaiorGrau < GrauNo.at(j)) MaiorGrau = GrauNo.at(j);

		vector<int> NosMaximos;
		for (int j = 0; j < GrauNo.size(); j++)
			if ((GrauNo.at(j) == MaiorGrau) && (MAux::Rede.at(j).get_TipoNo() == Node::Transparente))
				NosMaximos.push_back(j);

		int NoEscolhido = NosMaximos.at(floor( General::uniforme(0, NosMaximos.size()) ));
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
			if ((MaiorGrau < CentralidadeNo.at(j)) && (MAux::Rede.at(j).get_TipoNo() == Node::Transparente))
				MaiorGrau = CentralidadeNo.at(j);

		vector<int> NosMaximos;
		for (int j = 0; j < CentralidadeNo.size(); j++)
			if (CentralidadeNo.at(j) == MaiorGrau) NosMaximos.push_back(j);

		int NoEscolhido = NosMaximos.at(floor( General::uniforme(0, NosMaximos.size()) ));
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
	MAux *MainAux = new MAux();
	Def *Config = new Def(NULL);
	MainAux->laNet = MainAux->LaNetMin;
	Simula_Rede(Config, MainAux);

	for (int i = 0; i < NumNoTransparentes; i++) {
		int MaiorGrau = MainAux->RP_TLP_NodeUsage[0];
		for (int j = 0; j < Def::Nnodes; j++)
			if (MaiorGrau < MainAux->RP_TLP_NodeUsage[j]) MaiorGrau = MainAux->RP_TLP_NodeUsage[j];

		vector<int> NosMaximos;
		for (int j = 0; j < Def::Nnodes; j++)
			if ((MainAux->RP_TLP_NodeUsage[j] == MaiorGrau) && (MAux::Rede.at(j).get_TipoNo() == Node::Transparente))
				NosMaximos.push_back(j);

		int NoEscolhido = NosMaximos.at(floor( General::uniforme(0, NosMaximos.size()) ));
		MAux::Rede.at(NoEscolhido).set_TipoNo( Node::Translucido );
		MAux::Rede.at(NoEscolhido).set_NumRegeneradores( NumRegeneradoresPorNo );
		MainAux->RP_TLP_NodeUsage[NoEscolhido] = -1;
	}

	MAux::FlagRP_TLP = false;
	delete MainAux;
	delete Config;
}

void Regeneradores::RP_SQP(int NumTotalRegeneradores, int NumRegeneradoresPorNo, int LNMax) {
	assert (NumTotalRegeneradores % NumRegeneradoresPorNo == 0);
	int NumNoTransparentes = NumTotalRegeneradores/NumRegeneradoresPorNo;
	assert(NumNoTransparentes <= Def::Nnodes);
	assert(LNMax > 0);

	SQP_LNMax = LNMax;
	MAux::FlagRP_SQP = true;
	MAux *MainAux = new MAux();
	Def *Config = new Def(NULL);
	MainAux->laNet = MainAux->LaNetMin;
	Simula_Rede(Config, MainAux);

	for (int i = 0; i < NumNoTransparentes; i++) {
		int MaiorGrau = MainAux->RP_SQP_NodeUsage[0];
		for (int j = 0; j < Def::Nnodes; j++)
			if (MaiorGrau < MainAux->RP_SQP_NodeUsage[j]) MaiorGrau = MainAux->RP_SQP_NodeUsage[j];

		vector<int> NosMaximos;
		for (int j = 0; j < Def::Nnodes; j++)
			if ((MainAux->RP_SQP_NodeUsage[j] == MaiorGrau) && (MAux::Rede.at(j).get_TipoNo() == Node::Transparente))
				NosMaximos.push_back(j);

		int NoEscolhido = NosMaximos.at(floor( General::uniforme(0, NosMaximos.size()) ));
		MAux::Rede.at(NoEscolhido).set_TipoNo( Node::Translucido );
		MAux::Rede.at(NoEscolhido).set_NumRegeneradores( NumRegeneradoresPorNo );
		MainAux->RP_SQP_NodeUsage[NoEscolhido] = -1;
	}

	MAux::FlagRP_SQP = false;
	delete MainAux;
	delete Config;
}

bool Regeneradores::RA_FLR(Route *route, long double BitRate, Def *Config, Event *evt) {
	assert(BitRate > 0);

	evt->Esquema = _64QAM; //Esse algoritmo sempre tenta criar as chamadas com 64QAM.

	int RegeneneradoresNecessarios = ceil( BitRate/BR );
	int NoS, NoX;
	int r = 0;
	int L_or, L_de;
	int NslotsUsed;
	int NslotsReq = SlotsReq(BitRate, evt->Esquema);
	int SI; //SlotInicial

	vector<int> SegmentosTransparentes;
	int numSegmentosTransparentes = 0;
	vector<EsquemaDeModulacao> Esquemas;
	SegmentosTransparentes.push_back( route->getNode(0) );

	for (int s = 0; s < route->getNhops(); s++) {
		NoS = route->getNode(s);
		for (int x = s + 1; x <= route->getNhops(); x++) {
			NoX = route->getNode(x); //descobre o x-esimo no da rota
			if (( MAux::Rede.at(NoX).get_NumRegeneradoresDisponiveis() >= RegeneneradoresNecessarios ) || (x == route->getNhops()-1)) {
				Route rotaQuebrada = *route->breakRoute( NoS, NoX );
				NslotsUsed = 0;
				SI = -1;
				TryToConnect(&rotaQuebrada, NslotsReq, NslotsUsed, SI, Config);
				if (SI != -1) { //Há espectro
					long double OSNR = AvaliarOSNR(&rotaQuebrada, Config);
					if (OSNR >= Def::getlimiarOSNR(evt->Esquema, BitRate)) { //Há qualidade
						if (x == route->getNhops()) { //destino
							SegmentosTransparentes.push_back(NoX);
							for (int i = 0; i < numSegmentosTransparentes; i++) {
								Route rotaQuebrada = *route->breakRoute( SegmentosTransparentes.at(i), SegmentosTransparentes.at(i+1) );
								TryToConnect(&rotaQuebrada, NslotsReq, NslotsUsed, SI, Config);

								assert(SI != -1);
								assert(NslotsReq == NslotsUsed);

								evt->conexao->setFirstSlot(i, SI);
								evt->conexao->setLastSlot(i, SI + NslotsReq);

								for(unsigned c = 0; c < rotaQuebrada.getNhops(); c++) {
									L_or = rotaQuebrada.getNode(c);
									L_de = rotaQuebrada.getNode(c+1);
									for(int s = evt->conexao->getFirstSlot(i); s < evt->conexao->getLastSlot(i); s++) {
										assert(Config->Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] == false);
										Config->Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] = true;
										//Os slots sao marcados como ocupados
									}
								}

								int NumReg = MAux::Rede.at( SegmentosTransparentes.at(i) ).solicitar_regeneradores(BitRate);
								assert(NumReg != 0); //Conseguiu solicitar os regeneradores

								evt->RegeneradoresUtilizados[ SegmentosTransparentes.at(i) ] = NumReg;
							}
							return true;
						} else {
							r = x; //atualiza ponto de regeneração
						}
					}
				} else {
					if (r != s) {
						numSegmentosTransparentes++;
						SegmentosTransparentes.push_back(NoS);
						Esquemas.push_back(_64QAM);
						s = r; //atualiza fonte
						x = r; //atualiza ponto de teste
					} else {
						return false; //chamada bloqueada.
					}
				}
			}
		}
	}
	return false;
}
