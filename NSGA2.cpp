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
	k = 2;

	Evolucao = new Geracao[G];
	criar_GeracaoInicial();
}

NSGA2::~NSGA2() {
	for (int i = 0; i < G; i++) {
		for (int j = 0; j < Evolucao[i].Populacao.size(); j++) {
			delete[] Evolucao[i].Populacao.at(j)->Gene;
		}
	}
	delete[] Evolucao;
}

void NSGA2::criar_GeracaoInicial() {
	long double Rand1;
	for (int i = 0; i < S; i++)
		Evolucao[0].Populacao.push_back(new Individuo{i});
	Evolucao[0].idGeracao = 0;
	for (int i = 0; i < S; i++)
		Evolucao[0].Populacao.at(i)->Gene = new long double[T];

	for (int i = 0; i < S; i++) {
		for (int j = 0; j < T; j++) {
			Rand1 = General::uniforme(0,1);
			if (Rand1 < 0.5)
				Evolucao[0].Populacao.at(i)->Gene[j] = 0;
			else {
				int RiMax = Def::getGrauNo(j)*Def::getSE();
				Evolucao[0].Populacao.at(i)->Gene[j] = ceil(General::uniforme(0,RiMax));
			}
		}
	}

	for (int i = 0; i < S; i++)
		evalFuncoesCusto(Evolucao[0].Populacao.at(i));
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
	int opex = 0;
	for (int i = 0; i < T; i++)
		if (I->Gene[i] != 0) opex++;
	return opex;
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
	bool Visitados[G->Populacao.size()];
	for (int i = 0; i < S; i++) Visitados[i] = false;

	while (nVisitados != G->Populacao.size()) {
		for (int i = 0;	i < G->Populacao.size(); i++) {
			bool NaoDominado = true;
			for (int j = 0; j < G->Populacao.size(); j++) {
				if (j == i) continue;
				if (Visitados[j]) continue;
				NaoDominado &= !A_Domina_B(G->Populacao.at(j), G->Populacao.at(i));
			}

			if (NaoDominado) {
				Visitados[i] = true;
				nVisitados++;
				G->Populacao.at(i)->Aptidao = nFrentes;
			}
		}

		for (int i = 0; i < G->Populacao.size(); i++)
			if (G->Populacao.at(i)->Aptidao == nFrentes) Visitados[i] = true;
		nFrentes++;
	}
	G->nFrentesPareto = nFrentes;
}

void NSGA2::evalcrowdDistance(Geracao *G) {
	for (int i = 0; i <= G->nFrentesPareto; i++) {

	}
}

void NSGA2::criar_Geracao(Geracao *G) {

}

NSGA2::Geracao* NSGA2::Selecao(Geracao *G) {
	Geracao P, Q;
	P = *G;
	while (Q.Populacao.size() < S)
		Q.Populacao.push_back( TorneioBinario(&P) );
	return &Q;
}

NSGA2::Individuo* NSGA2::TorneioBinario(Geracao *G) {
	Individuo *I = G->Populacao.at( General::uniforme(0,G->Populacao.size()-1) );
	Individuo *J;
	for (int i = 1; i < k; i++) {
		J = G->Populacao.at( General::uniforme(0,G->Populacao.size()-1) );
		if (I->Aptidao > J->Aptidao)
			I = J;
	}
	G->Populacao.erase( G->Populacao.begin() + I->id );
}
