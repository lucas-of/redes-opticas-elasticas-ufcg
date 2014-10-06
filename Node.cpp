#include "Node.h"
#include "General.h"
#include "Def.h"
#include "Constantes.h"

Node::Node(int who) {
    assert (who>=0);
    whoami = who;
    loss = General::dB(-1*Def::get_Lsss());
    calcula_ganho_preamp();
    calcula_ganho_pot();
}

int Node::get_whoami() {
    return whoami;
}

void Node::calcula_ganho_preamp() {
    if (Def::get_Arquitetura() == Def::SS) {
        gain_preamp = General::dB(Def::get_Lsss());
    } else if (Def::get_Arquitetura() == Def::BS) {
        gain_preamp = Def::getGrauNo(get_whoami());
    }
    calcula_ruido_preamp();
}

void Node::calcula_ganho_pot() {
    gain_pot = General::dB(Def::get_Lsss());
    calcula_ruido_pot();
}

void Node::calcula_ruido_preamp() {
    double freq = Constante::c/Def::getlambda();
    ruido_preamp = 0.5*Constante::h*freq*Def::get_Bslot()*gain_preamp*Def::get_Famp();
}

void Node::calcula_ruido_pot() {
    double freq = Constante::c/Def::getlambda();
    ruido_pot = 0.5*Constante::h*freq*Def::get_Bslot()*gain_pot*Def::get_Famp();
}

long double Node::get_gain_preamp() {
    return gain_preamp;
}

long double Node::get_loss() {
    return loss;
}

long double Node::get_ruido_preamp(int nslots) {
    return nslots*ruido_preamp;
}

long double Node::get_gain_pot() {
    return gain_pot;
}

long double Node::get_ruido_pot(int nslots) {
    return nslots*ruido_pot;
}
