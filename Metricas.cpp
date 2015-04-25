#include "Def.h"
#include "Main_Auxiliar.h"

long double ProbBloqueio(Def *Config) {
	return Config->numReq_Bloq/Config->numReq;
}

long double ProbAceitacao(Def *Config) {
	return 1.0 - ProbBloqueio(Config);
}

void ProbBloqueioTaxa(Def *Config) {
	for (int i = 0; i < Def::get_numPossiveisTaxas(); i++ ) {
		MAux::ResulProbBloqTaxa << MAux::laNet << "\t" << Def::PossiveisTaxas[i] << "\t" << Config->numReqBloq_Taxa[i]/Config->numReq_Taxa[i] << endl;
		//cout << Def::PossiveisTaxas[i]/pow(10,9) << "Gbps\tProb Bloq = " << Def::numReqBloq_Taxa[i]/Def::numReq_Taxa[i] << endl;
	}
}

void ProbAceitacaoTaxa(Def *Config) {
	for (int i = 0; i < Def::get_numPossiveisTaxas(); i++ ) {
		MAux::ResulProbAceitTaxa << MAux::laNet << "\t" << Def::PossiveisTaxas[i] << "\t" << 1.0 - Config->numReqBloq_Taxa[i]/Config->numReq_Taxa[i] << endl;
	}
}

void calcTaxaMedia(Def *Config) {
	long double TaxaMedia = 0;
	for (int i = 0; i < Def::get_numPossiveisTaxas(); i++) {
		TaxaMedia += Def::PossiveisTaxas[i] * Config->tempoTotal_Taxa[i];
	}
	TaxaMedia /= MAux::simTime;
	cout << "Taxa Media de transmissão é " << TaxaMedia/pow(10,9) << " Gbps" << endl;
	MAux::ResulTaxaMedia << MAux::laNet << "\t" << TaxaMedia << endl;
}

void AceitacaoEsquema(Def *Config) {
	for (int i = 0; i < Def::numEsquemasDeModulacao; i++ ) {
		MAux::ResultEsquemas << i << "\t" << Config->numReqAceit_Esquema[i]/Config->numReq << "\t" << Config->taxaTotal_Esquema[i]/Config->taxaTotal << endl;
	}
}
