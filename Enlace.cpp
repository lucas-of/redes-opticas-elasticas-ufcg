#include "Enlace.h"
#include "math.h"
#include "Def.h"
#include "PSR.h"
#include "assert.h"
#include "Constantes.h"

Enlace::Enlace(Node *NOrig, Node *NDest, double dist) {
	assert (dist > 0);
	Origem = NOrig;
	Destino = NDest;
	distancia = dist;
	calcula_perdas();
	if (NOrig != NULL) { //se nao e o no "infinito"
		calcula_num_amplificadores(MAux::Config);
		calcula_ganho_enlace();
		calcula_ruido_enlace();
		calcula_preamplif(MAux::Config);
	}
}

void Enlace::calcula_num_amplificadores(Def *Config) {
	num_amplif = floor(distancia/Config->get_DistaA());
	if (ceil(distancia/Config->get_DistaA()) == num_amplif) num_amplif--;
}

void Enlace::calcula_ganho_enlace() {
	ganho_enlace = General::dB(1.0*num_amplif*(General::lin(L_DCF)+General::lin(L_FB))/(1.0*num_amplif + 1.0));
}

long double Enlace::get_ganho_enlace() {
	return ganho_enlace;
}

void Enlace::calcula_ruido_enlace() {
	if (num_amplif == 0) ruido_enlace = 0;
	else {
		double freq = Def::get_freq();

		ruido_enlace = num_amplif*Def::get_Famp()*Constante::h*freq*Def::get_Bref();
		ruido_enlace *= (1.0L - pow(L_FB*L_DCF, -1.0/(num_amplif+1.0)));
	}
}

void Enlace::calcula_perdas() {
	long double dDCF = 0;
	dDCF = (dDCF > 0 ? dDCF : -dDCF);
	L_FB = pow(10,0.1*Constante::alphaFB*distancia);//exp(Constante::alphaFB*distancia/4.34);
	L_DCF = pow(10,0.1*Constante::alphaDCF*dDCF); //exp(Constante::alphaDCF*dDCF/4.34);
 }

long double Enlace::get_ruido_enlace() {
	return ruido_enlace;
}

double Enlace::get_comprimento() {
	return distancia;
}

long double Enlace::get_perda_enlace() {
	return L_FB*L_DCF;
}

long double Enlace::get_ganho_preamplif() {
	return ganho_preamplif;
}

long double Enlace::get_ruido_preamplif() {
	return ruido_preamplif;
}

void Enlace::calcula_preamplif(Def *Config) {
	calcula_perdas();
	long double freq = Def::get_freq();

	if (Def::get_Arquitetura() == Def::SS) {
		ganho_preamplif = General::dB((General::lin(L_FB)/(num_amplif + 1.0)) + Def::get_Lsss());
	} else if (Def::get_Arquitetura() == Def::BS) {
		ganho_preamplif = General::dB((General::lin(L_FB)/(num_amplif + 1.0)) + 10.0*log10( Config->getGrauNo(Destino->whoami) + 1 ));
	}

	ruido_preamplif = Def::get_Famp()*(ganho_preamplif - 1.0)*Constante::h*freq*Def::get_Bref();
}

void Enlace::recalcular(Def *Config) {
	calcula_perdas();
	if (Origem != NULL) { //se nao e o no "infinito"
		calcula_num_amplificadores(Config);
		calcula_ganho_enlace();
		calcula_ruido_enlace();
		calcula_preamplif(Config);
	}
}

long double Enlace::get_peso(Def *Config, int L, long double *PartCoef, long double Noise) {
	long double peso = 0;
	long double SlotsDispon = 0;
	if (PSR::C == PSR::DistanciaNumFormas) {
		bool *Disp = new bool[Def::getSE()];
		for (int Slot = 0; Slot < Def::getSE(); Slot++)
			Disp[Slot] = !Config->Topology_S[Slot*Def::Nnodes*Def::Nnodes + Def::Nnodes*Origem->whoami + Destino->whoami];
		SlotsDispon = Heuristics::calcNumFormAloc(L,Disp);
		delete[] Disp;
	} else if (PSR::C == PSR::DistanciaDisponibilidade)
		for (int Slot = 0; Slot < Def::getSE(); Slot++)
			if (!Config->Topology_S[Slot*Def::Nnodes*Def::Nnodes + Def::Nnodes*Origem->whoami + Destino->whoami])
				SlotsDispon += 1;

	for (int i = PSR::get_NMin(); i <= PSR::get_NMax() ; i++) {
		for (int j = PSR::get_NMin(); j <= PSR::get_NMax(); j++) {
			if ((PSR::C == PSR::DistanciaDisponibilidade) || (PSR::C == PSR::DistanciaNumFormas))
				if (PartCoef !=  NULL)
					peso += PartCoef[(i-PSR::get_NMin())*PSR::get_N()+(j-PSR::get_NMin())]*PSR::get_Disponibilidade(SlotsDispon,i)*PSR::get_Distancia(Origem->whoami, Destino->whoami, j);
				else
					peso += Coeficientes[(i-PSR::get_NMin())*PSR::get_N()+(j-PSR::get_NMin())]*PSR::get_Disponibilidade(SlotsDispon,i)*PSR::get_Distancia(Origem->whoami, Destino->whoami, j);
			else {
				if (PartCoef !=  NULL)
					peso += PartCoef[(i-PSR::get_NMin())*PSR::get_N()+(j-PSR::get_NMin())]*PSR::get_Disponibilidade(SlotsDispon,i)*pow(Noise,j);
				else
					peso += Coeficientes[(i-PSR::get_NMin())*PSR::get_N()+(j-PSR::get_NMin())]*PSR::get_Disponibilidade(SlotsDispon,i)*pow(Noise,j);
			}
		}
	}
	return peso;
}

void Enlace::recalcular_peso(long double *Coef) {
	Coeficientes = Coef;
}

Node* Enlace::get_NodeOrigem() {
	return Origem;
}

void Enlace::set_distancia(long double dist) {
	assert (dist > 0);
	distancia = dist;
}
