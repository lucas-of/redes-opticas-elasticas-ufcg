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
    if (NOrig != NULL) {
        calcula_num_amplificadores(dist);
        calcula_ganho_enlace_indiv();
        calcula_ganho_enlace();
        calcula_ruido_enlace();
    }
}

void Enlace::calcula_num_amplificadores(double dist) {
    num_amplif = floor(dist/Def::get_DistaA());
}

void Enlace::calcula_ganho_enlace_indiv() {
    ganho_enlace_indiv = pow(Def::get_LDCF()*Def::get_LFB(),1.0/num_amplif);
}

void Enlace::calcula_ganho_enlace() {
    ganho_enlace = Def::get_LDCF()*Def::get_LFB();
}

long double Enlace::get_ganho_enlace() {
    return ganho_enlace;
}

void Enlace::calcula_ruido_enlace() {
    if (num_amplif == 0) ruido_enlace = 0;
    else {
        ruido_enlace = Constante::h*Constante::c*Def::getSE()*Def::get_Bslot()*Def::get_Famp()/(2.0 * Def::getlambda());
        long double sum = 0;
        for (int k = 1; k<= num_amplif ; k++) {
            sum += pow(ganho_enlace_indiv,num_amplif+1-k)/pow(Def::get_LFB()*Def::get_LDCF(), (num_amplif+1.0-k)/(1.0*(num_amplif + 1)));
        }
        ruido_enlace *= sum;
    }
}

long double Enlace::get_ruido_enlace() {
    return ruido_enlace;
}
