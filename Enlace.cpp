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
    calcula_perdas();
    if (NOrig != NULL) { //se nao e o no "infinito"
        calcula_num_amplificadores();
        calcula_ganho_enlace_indiv();
        calcula_ganho_enlace();
        calcula_ruido_enlace();
    }
}

void Enlace::calcula_num_amplificadores() {
    num_amplif = floor(distancia/Def::get_DistaA());
}

void Enlace::calcula_ganho_enlace_indiv() {
    ganho_enlace_indiv = pow(L_DCF*L_FB,1.0/num_amplif);
}

void Enlace::calcula_ganho_enlace() {
    ganho_enlace = L_DCF*L_FB;
}

long double Enlace::get_ganho_enlace() {
    return ganho_enlace;
}

void Enlace::calcula_ruido_enlace() {
    if (num_amplif == 0) ruido_enlace = 0;
    else {
        double freq = Constante::c/Def::getlambda();
        ruido_enlace = Constante::h*freq*Def::get_Bslot()*Def::get_Famp()*0.5;
        long double sum = 0;
        for (int k = 1; k<= num_amplif ; k++) {
            sum += pow(ganho_enlace_indiv,num_amplif+1-k)/pow(L_FB*L_DCF, ((num_amplif+1.0-k)/(num_amplif + 1.0)));
        }
        ruido_enlace *= sum;
    }
}

void Enlace::calcula_perdas() {
    long double dDCF = (distancia*Constante::Dcr/Constante::DDCF);
    dDCF = (dDCF > 0 ? dDCF : -dDCF);
    L_FB = exp(Constante::alphaFB*distancia/4.34);
    L_DCF = exp(Constante::alphaDCF*dDCF/4.34);
}

long double Enlace::get_ruido_enlace(int num_slots) {
    assert (num_slots > 0);
    assert (num_slots <= Def::getSE());
    return num_slots*ruido_enlace;
}

double Enlace::get_comprimento() {
    return distancia;
}
