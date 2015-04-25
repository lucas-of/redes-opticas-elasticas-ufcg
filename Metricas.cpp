#include "Def.h"
#include "Main_Auxiliar.h"

long double ProbBloqueio() {
	return Def::numReq_Bloq/Def::numReq;
}

long double ProbAceitacao() {
	return 1.0 - ProbBloqueio();
}

void ProbBloqueioTaxa() {
	for (int i = 0; i < Def::get_numPossiveisTaxas(); i++ ) {
		MAux::ResulProbBloqTaxa << MAux::laNet << "\t" << Def::PossiveisTaxas[i] << "\t" << MAux::Config.numReqBloq_Taxa[i]/MAux::Config.numReq_Taxa[i] << endl;
		//cout << Def::PossiveisTaxas[i]/pow(10,9) << "Gbps\tProb Bloq = " << Def::numReqBloq_Taxa[i]/Def::numReq_Taxa[i] << endl;
	}
}

void ProbAceitacaoTaxa() {
	for (int i = 0; i < Def::get_numPossiveisTaxas(); i++ ) {
		MAux::ResulProbAceitTaxa << MAux::laNet << "\t" << Def::PossiveisTaxas[i] << "\t" << 1.0 - MAux::Config.numReqBloq_Taxa[i]/MAux::Config.numReq_Taxa[i] << endl;
	}
}

void calcTaxaMedia() {
	long double TaxaMedia = 0;
	for (int i = 0; i < Def::get_numPossiveisTaxas(); i++) {
		TaxaMedia += Def::PossiveisTaxas[i] * Def::tempoTotal_Taxa[i];
	}
	TaxaMedia /= MAux::simTime;
	cout << "Taxa Media de transmissão é " << TaxaMedia/pow(10,9) << " Gbps" << endl;
	MAux::ResulTaxaMedia << MAux::laNet << "\t" << TaxaMedia << endl;
}

void AceitacaoEsquema() {
	for (int i = 0; i < Def::numEsquemasDeModulacao; i++ ) {
		MAux::ResultEsquemas << i << "\t" << MAux::Config.numReqAceit_Esquema[i]/MAux::Config.numReq << "\t" << MAux::Config.taxaTotal_Esquema[i]/MAux::Config.taxaTotal << endl;
	}
}
