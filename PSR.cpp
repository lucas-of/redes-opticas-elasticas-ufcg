#include "PSR.h"
#include "Main_Auxiliar.h"
int PSR::N;
long double **PSR::Coeficientes, **PSR::DisponibilidadeNormalizada, **PSR::ComprimentosNormalizados;
long double PSR::MaiorEnlace;

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
