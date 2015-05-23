#include "NSGA2.h"
#include "Main_Auxiliar.h"
#include "Def.h"

int NSGA2::G;
int NSGA2::S;
int NSGA2::T;
NSGA2::Geracao* NSGA2::Evolucao;

NSGA2::NSGA2() {
	G = 300;
	S = 100;
	T = Def::getNnodes();

	g = 0;

	Evolucao = new Geracao[G];
	criar_GeracaoInicial();
}

NSGA2::~NSGA2() {
	for (int i = 0; i < G; i++) {
		for (int j = 0; j < S; j++) {
			if (Evolucao[i].Populacao != NULL)
				delete[] Evolucao[i].Populacao[j].Gene;
		}
		delete[] Evolucao[i].Populacao;
	}
	delete[] Evolucao;
}

void NSGA2::criar_GeracaoInicial() {
	long double Rand1, Rand2;
	Evolucao[0].Populacao = new Individuo[S];
	Evolucao[0].idGeracao = 0;
	for (int i = 0; i < S; i++)
		Evolucao[0].Populacao[i].Gene = new long double[T];

	for (int i = 0; i < S; i++) {
		for (int j = 0; j < T; j++) {
			long double Rand1 = General::uniforme(0,1);
			if (Rand1 < 0.5)
				Evolucao[0].Populacao[i].Gene[j] = 0;
			else {
				int RiMax = Def::getGrauNo(j)*Def::getSE();
				Evolucao[0].Populacao[i].Gene[j] = ceil(General::uniforme(0,RiMax));
			}
		}
	}

	for (int i = 0; i < S; i++)
		evalFuncoesCusto(&Evolucao[0].Populacao[i]);
}

void NSGA2::evalFuncoesCusto(Individuo *I) {
	I->CapEx = evalCapEx(I);
	I->NNT = evalNNT(I);
	I->PbReq = evalPbReq(I);
}

long double NSGA2::evalCapEx(Individuo *I) {
	int capex = 0;
	for (int i = 0; i < T; i++)
		capex += I->Gene[i];
	return capex;
}

long double NSGA2::evalNNT(Individuo *I) {
	int nnt = 0;
	for (int i = 0; i < T; i++)
		if (I->Gene[i] != 0) nnt++;
	return nnt;
}

long double NSGA2::evalPbReq(Individuo *I) {
	return 0; //esperando as meninas
}
