#include "NSGA2.h"
#include "Main_Auxiliar.h"
#include "Def.h"

int NSGA2::G;
int NSGA2::S;
int NSGA2::T;
int NSGA2::RiMax;
NSGA2::Geracao* NSGA2::Evolucao;

NSGA2::NSGA2(int nG) {
	assert(nG > 0);
	G = nG;

	Evolucao = new Geracao[G];
	criar_GeracaoInicial();
}

NSGA2::~NSGA2() {
	for (int i = 0; i < G; i++) {
		for (int j = 0; j < S; j++) {
			delete[] Evolucao[i].Populacao[j].Gene;
		}
		delete[] Evolucao[i].Populacao;
	}
	delete[] Evolucao;
}

void NSGA2::criar_Geracao(int numGeracao) {
	assert(numGeracao > 1);
}

void NSGA2::criar_GeracaoInicial() {
	long double Rand1, Rand2;
	Evolucao[0].Populacao = new Individuo[S];
	for (int i = 0; i < S; i++)
		Evolucao[0].Populacao[i].Gene = new long double[T];

	for (int i = 0; i < S; i++) {
		for (int j = 0; j < T; j++) {
			long double Rand1 = General::uniforme(0,1);
			if (Rand1 < 0.5)
				Evolucao[0].Populacao[i].Gene[j] = 0;
			else
				Evolucao[0].Populacao[i].Gene[j] = ceil(General::uniforme(0,RiMax));
		}
	}
}
