#include "Node.h"
#include "General.h"
#include "Def.h"
#include "Constantes.h"

Node::Node(int who) {
	assert (who>=0);
	whoami = who;
	loss = General::dB(-1*Def::get_Lsss());
}

int Node::get_whoami() {
	return whoami;
}

void Node::calcula_ganho_pot() {
	gain_pot = General::dB( General::lin(Def::get_Pref()/1E-3L) - potencia_tx + Def::get_Lsss());
	calcula_ruido_pot();
}

void Node::calcula_ruido_pot() {
	long double freq = Def::get_freq();
	ruido_pot = Def::get_Famp()*(gain_pot-1.0)*Constante::h*freq*Def::get_Bref();
}

long double Node::get_loss() {
	return loss;
}

long double Node::get_gain_pot() {
	calcula_ganho_pot();
	return gain_pot;
}

long double Node::get_ruido_pot(int) {
	calcula_ganho_pot();
	return ruido_pot;
}

void Node::set_potenciatx(long double potencia) {
	//Potencia em dBm
	potencia_tx = potencia;
}
