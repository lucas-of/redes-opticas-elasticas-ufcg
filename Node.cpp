#include "Node.h"
#include "Def.h"

Node::Node() {
    
}

void Node::calc_Nin(){//Ruido do amplificar
    Nin = Def::get_Pin() / Def::get_OSRNin();
}

void Node::calc_Nnz(){//posição do amplificador
    Nnz=123;
}
