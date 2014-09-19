#include "Enlace.h"
#include "math.h"
#include "Def.h"
#include "assert.h"
#include "Constantes.h"

Enlace::Enlace(Node *NOrig, Node *NDest, double dist) {
    assert (dist > 0);
    Origem = NOrig;
    Destino = NDest;
    distancia = dist;
    calcula_num_amplificadores(dist);
    calcula_ganho_enlace();
}

void Enlace::calcula_num_amplificadores(double dist) {
    num_amplif = floor(dist/Def::get_DistaA());
}

void Enlace::calcula_ganho_enlace() {
    ganho_enlace = pow(Def::get_LDCF()*Def::get_LFB(),1.0/num_amplif);
}

long double Enlace::get_ganho_enlace() {
    return ganho_enlace;
}

void Enlace::calcula_Nnz(){
    Nnz = (Constante::h*Constante::c*Def::get_Bslot()*Def::getSE()*ganho_enlace*Def::get_Famp()/(2*Def::getlambda()));
}

long double Enlace::get_Nnz(){
    return Nnz;
}
