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
        gain_preamp = Def::getGrauNo(get_whoami())+1;
    }
    calcula_ruido_preamp();
}

void Node::calcula_ganho_pot() {
    gain_pot = General::dB(Def::get_Lsss());
    calcula_ruido_pot();
}

void Node::calcula_ruido_preamp() {
    long double freq = Constante::c/Def::getlambda();
    ruido_preamp = Def::get_Famp()*(gain_preamp-1.0)*Constante::h*freq*Def::get_Bslot();
}

void Node::calcula_ruido_pot() {
    long double freq = Constante::c/Def::getlambda();
    ruido_pot = Def::get_Famp()*(gain_pot-1.0)*Constante::h*freq*Def::get_Bslot();
}

long double Node::get_gain_preamp() {
    return gain_preamp;
}

long double Node::get_loss() {
    return loss;
}

long double Node::get_ruido_preamp(int nslots) {
    assert (nslots > 0);
    assert (nslots <= Def::getSE());
    return ruido_preamp;
}

long double Node::get_gain_pot() {
    return gain_pot;
}

long double Node::get_ruido_pot(int nslots) {
    assert (nslots > 0);
    assert (nslots <= Def::getSE());
    return ruido_pot;
}
