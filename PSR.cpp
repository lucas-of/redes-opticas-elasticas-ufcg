#include "PSR.h"
#include "Main_Auxiliar.h"
int PSR::N;
long double **PSR::Coeficientes, **PSR::DisponibilidadeNormalizada, **PSR::ComprimentosNormalizados;
Particula *PSR::PSO_populacao, *Melhor = NULL;
long double PSR::MaiorEnlace = -1;
int PSR::PSO_P, PSR::PSO_G;
long double PSR::PSO_c1, PSR::PSO_c2, PSR::PSO_chi;

PSR::PSR(int NewN) {
    assert(NewN > 0);
	MaiorEnlace = 0;
	N = NewN;

	Coeficientes = new long double*[PSR::get_N()];
	for (int i = 0; i < N; i++) Coeficientes[i] = new long double[PSR::get_N()];

	DisponibilidadeNormalizada = new long double*[Def::getNnodes()];
	ComprimentosNormalizados = new long double*[Def::getNnodes()];
	for (int i = 0; i < Def::getNnodes(); i++) {
		DisponibilidadeNormalizada[i] = new long double[Def::getNnodes()];
		ComprimentosNormalizados[i] = new long double[Def::getNnodes()];
	}
    Normalizacao();
}

PSR::~PSR() {
	delete[] Coeficientes;
}

const int PSR::get_N() {
	return PSR::N;
}

long double PSR::get_coeficiente(int i, int j) {
	assert ((i < get_N()) && (i >= 0));
	assert ((j < get_N()) && (j >= 0));
	return Coeficientes[i][j];
}

long double PSR::procurarMaiorEnlace() {
	for (int i = 0; i < Def::getNnodes(); i++) {
		for (int j = 0; j < Def::getNnodes(); j++) {

			if (MAux::Caminho[i].at(j).get_NodeOrigem() == NULL) continue;
			if (MaiorEnlace < MAux::Caminho[i].at(j).get_comprimento()) MaiorEnlace = MAux::Caminho[i].at(j).get_comprimento();
		}
	}
	return MaiorEnlace;
}

void PSR::Normalizacao() {
	bool Disp[Def::getSE()];
	procurarMaiorEnlace();

	for (int i = 0; i < Def::getNnodes(); i++) {
		for (int j = 0; j < Def::getNnodes(); j++) {
			if (MAux::Topology[i][j] == 0) {
				ComprimentosNormalizados[i][j] = Def::MAX_LONGDOUBLE;
				DisponibilidadeNormalizada[i][j] = Def::MAX_LONGDOUBLE;
				continue;
			}
			ComprimentosNormalizados[i][j] = MAux::Caminho[i].at(j).get_comprimento() / MaiorEnlace;
			for (int Slot = 0; Slot < Def::getSE(); Slot++)
				Disp[Slot] = !MAux::Topology_S[Slot][i][j];
			DisponibilidadeNormalizada[i][j] = Heuristics::calcNumFormAloc( 1, Disp ) / Def::getSE();
		}
	}
}

void PSR::PSO_configurar() {
	PSO_P = 50;
	PSO_G = 500;
	PSO_c1 = 2.05;
	PSO_c2 = 2.05;

	long double phi = PSO_c1 + PSO_c2;
	PSO_chi = 2.0/(2 - phi - sqrt(phi*phi - 4*phi));

	PSO_populacao = new Particula[PSO_P];
	for (int i = 0; i < PSO_P; i++) {
		PSO_populacao[i].x = new long double*[N];
		PSO_populacao[i].v = new long double*[N];
		for (int j = 0; j < N; j++) {
			PSO_populacao[i].x[j] = new long double[N];
			PSO_populacao[i].v[j] = new long double[N];
		}
	}
}

void PSR::PSO() {
	PSO_configurar();
	PSO_iniciarPopulacao();
}

void PSR::PSO_iniciarPopulacao() {
	for (int i = 0; i < PSO_P; i++) {
		for (int j = 0; j < N; j++) {
			PSO_populacao[i].v[j] = {0}; //velocidade inicialmente nula
			for (int k = 0; k < N; k++)
				PSO_populacao[i].x[j][k] = General::uniforme(0,1);
		}
	}
}

void PSR::PSO_atualizaCustoEnlaces(Particula P) {
    for (int i = 0; i < Def::getNnodes(); i++) {
        for (int j = 0; j < Def::getNnodes(); j++) {
            if (MAux::Topology[i][j] == 1)
                MAux::Caminho[i].at(j).recalcular_peso(P.x);
        }
    }
}

long double PSR::get_MaiorEnlace() {
    if (MaiorEnlace==-1) procurarMaiorEnlace();
    return MaiorEnlace;
}

void PSR::atualizaDisponibilidade() {
    bool Disp[Def::getSE()];
    for (int i = 0; i < Def::getNnodes(); i++) {
        for (int j = 0; j < Def::getNnodes(); j++) {
            for (int Slot = 0; Slot < Def::getSE(); Slot++)
                Disp[Slot] = !MAux::Topology_S[Slot][i][j];
            DisponibilidadeNormalizada[i][j] = Heuristics::calcNumFormAloc( 1, Disp ) / Def::getSE();
        }
    }
}
