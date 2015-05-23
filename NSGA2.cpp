#include "NSGA2.h"
#include "Main_Auxiliar.h"
#include "Def.h"

int NSGA2::G;
int NSGA2::S;
int NSGA2::T;
long double NSGA2::Pc;
long double NSGA2::Pm;
NSGA2::Geracao* NSGA2::Evolucao;

NSGA2::NSGA2() {
	G = 300;
	S = 100;
	T = Def::getNnodes();

	Pm = 0.1;
	Pc = 1;

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
	I->Objetivos[PbReq] = evalPbReq(I);
	I->Objetivos[OpEx] = evalOpEx(I);
	I->Objetivos[CapEx] = evalCapEx(I);
	I->Objetivos[Energia] = evalEnergia(I);
}

long double NSGA2::evalCapEx(Individuo *I) {
	int capex = 0;
	for (int i = 0; i < T; i++)
		capex += I->Gene[i];
	return capex;
}

long double NSGA2::evalOpEx(Individuo *I) {
	return 0; //esperando helder
}

long double NSGA2::evalEnergia(Individuo *I) {
	return 0; //esperando helder
}

long double NSGA2::evalPbReq(Individuo *I) {
	return 0; //esperando as meninas
}

bool NSGA2::A_Domina_B(Individuo *A, Individuo *B) {
	for (int i = 0; i < ParamMAX; i++)
		if (A->Objetivos[i] < B->Objetivos[i]) {
			bool Domina = true;
			for (int j = 0; j < ParamMAX; j++) {
				if (i == j) continue;
				Domina &= (A->Objetivos[j] <= B->Objetivos[j]);
			}
			if (Domina) return true;
		}
	return false;
}

void NSGA2::evalPareto(Geracao *G) {
	int nVisitados = 0;
	int nFrentes = 0;
	bool Visitados[S];
	for (int i = 0; i < S; i++) Visitados[i] = false;

	while (nVisitados != S) {
		for (int i = 0;	i < S; i++) {
			bool NaoDominado = true;
			for (int j = 0; j < S; j++) {
				if (j == i) continue;
				if (Visitados[j]) continue;
				NaoDominado &= !A_Domina_B(&G->Populacao[j], &G->Populacao[i]);
			}

			if (NaoDominado) {
				Visitados[i] = true;
				nVisitados++;
				G->Populacao[i].Aptidao = nFrentes;
			}
		}

		for (int i = 0; i < S; i++)
			if (G->Populacao[i].Aptidao == nFrentes) Visitados[i] = true;
		nFrentes++;
	}

}
