#include "Node.h"
#include "Def.h"

Node::Node(int who) {
    assert (who>=0);
    whoami = who;
}

void Node::calc_Nin(){//Ruido do amplificar
    Nin = Def::get_Pin() / Def::get_OSRNin();
}

int Node::get_whoami() {
    return whoami;
}
