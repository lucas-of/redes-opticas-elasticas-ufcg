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

void Node::calcula_ganho_pot(Def *Config) {
	gain_pot = General::dB( General::lin(Config->get_Pref()/1E-3L) - potencia_tx + Config->get_Lsss());
	calcula_ruido_pot();
}

void Node::calcula_ruido_pot() {
	long double freq = Def::get_freq();
	ruido_pot = Def::get_Famp()*(gain_pot-1.0)*Constante::h*freq*Def::get_Bref();
}

long double Node::get_loss() {
	return loss;
}

long double Node::get_gain_pot(Def *Config) {
	calcula_ganho_pot(Config);
	return gain_pot;
}

long double Node::get_ruido_pot(Def *Config) {
	calcula_ganho_pot(Config);
	return ruido_pot;
}

void Node::set_potenciatx(long double potencia) {
	//Potencia em dBm
	potencia_tx = potencia;
    
}

void Node::set_regenerador(int x){
    if(x%2!=0){
        reg=1;
    }
}

int Node::get_regenerador(){
    return reg;
}


int Node::available_regenerators(int c){
      while(c!=0){
      return 1;
      }
}

void Node::set_n_used_regenerators(int Br){
    ur = Br/100; //Considerando-se a taxa de 100Gb/s para regeneradores
}

int Node::get_n_used_regenerators(){// Retorna numero de regeneradores utilizados
    return ur;
}

void Node::set_transp_seg(int s, int r, int x){
    if(x%2 == 0){
        if(x>=s && x<=r){
            if(MAux::Topology[orN*Def::getNnodes()+deN] == 1){
      transp = 1;
            }

        }
    }
}

int Node::get_transp_seg (){
    return transp;
}
