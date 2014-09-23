#include "Node.h"
#include "Def.h"
#include "Constantes.h"

Node::Node(int who) {
    assert (who>=0);
    whoami = who;
    loss = 1.0/Def::get_Lsss();
    calcula_ganho_preamp();
    calcula_ganho_pot();
}

int Node::get_whoami() {
    return whoami;
}

void Node::calcula_ganho_preamp() {
    if (Def::get_Arquitetura() == Def::SS) {
        gain_preamp = Def::get_Lsss();
    } else if (Def::get_Arquitetura() == Def::BS) {
        gain_preamp = Def::getGrauNo(get_whoami());
    }
    calcula_ruido_preamp();
}

void Node::calcula_ganho_pot() {
    gain_pot = Def::get_Lsss();
    calcula_ruido_pot();
}

void Node::calcula_ruido_preamp() {
    ruido_preamp = (Constante::h*Constante::c*Def::get_Bslot()*Def::getSE()*gain_preamp*Def::get_Famp()/(2*Def::getlambda()));
}

void Node::calcula_ruido_pot() {
    ruido_pot = (Constante::h*Constante::c*Def::get_Bslot()*Def::getSE()*gain_pot*Def::get_Famp()/(2*Def::getlambda()));
}

long double Node::get_gain_preamp() {
    return gain_preamp;
}

long double Node::get_loss() {
    return loss;
}

long double Node::get_ruido_preamp() {
    return ruido_preamp;
}

long double Node::get_gain_pot() {
    return gain_pot;
}

long double Node::get_ruido_pot() {
    return ruido_pot;
}
