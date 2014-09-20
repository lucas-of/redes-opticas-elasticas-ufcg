#include "Node.h"
#include "Def.h"
#include "Constantes.h"

Node::Node(int who) {
    assert (who>=0);
    whoami = who;
    loss = 1.0/Def::get_Lsss();
    calcula_ganho();
}

void Node::calc_Nin(){//Ruido do amplificar
    Nin = Def::get_Pin() / Def::get_OSRNin();
}

int Node::get_whoami() {
    return whoami;
}

void Node::calcula_ganho() {
    if (Def::get_Arquitetura() == Def::SS) {
        gain = Def::get_Lsss();
    } else if (Def::get_Arquitetura() == Def::BS) {
        gain = Def::getGrauNo(get_whoami());
    }
    calcula_ruido();
}

void Node::calcula_ruido() {
    noise = (Constante::h*Constante::c*Def::get_Bslot()*Def::getSE()*gain*Def::get_Famp()/(2*Def::getlambda()));
}

long double Node::get_gain() {
    return gain;
}

long double Node::get_loss() {
    return loss;
}

long double Node::get_noise() {
    return noise;
}
