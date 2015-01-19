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
    if ((int) (distancia/Def::get_DistaA()) == num_amplif) num_amplif--;
}

void Enlace::calcula_ganho_enlace_indiv() {
    if (num_amplif != 0)
        ganho_enlace_indiv = pow(L_DCF*L_FB,1.0/num_amplif);
    else
        ganho_enlace_indiv = 1;
}

void Enlace::calcula_ganho_enlace() {
    if (num_amplif != 0)
        ganho_enlace = L_DCF*L_FB;
    else
        ganho_enlace = 1;
}

long double Enlace::get_ganho_enlace() {
    return ganho_enlace;
}

void Enlace::calcula_ruido_enlace() {
    if (num_amplif == 0) ruido_enlace = 0;
    else {
        double freq = Constante::c/Def::getlambda();

        ruido_enlace = 0.5*Def::get_Famp()*Constante::h*freq*Def::get_Bslot();
        ruido_enlace *= (-1.0 + pow(L_FB*L_DCF, 1.0/(num_amplif)));
        ruido_enlace /= pow(L_FB*L_DCF, 1.0/(num_amplif + 1.0));

        long double sum = 0;
        for (int k = 1; k <= num_amplif; k++) {
            sum += pow(L_FB*L_DCF, (k - 1.0)/(num_amplif * (num_amplif + 1.0)));
        }
        ruido_enlace *= sum;
    }
}

void Enlace::calcula_perdas() {
    long double dDCF = (distancia*Constante::Dcr/Constante::DDCF);
    dDCF = (dDCF > 0 ? dDCF : -dDCF);
    L_FB = pow(10,0.1*Constante::alphaFB*distancia);//exp(Constante::alphaFB*distancia/4.34);
    L_DCF = pow(10,0.1*Constante::alphaDCF*dDCF); //exp(Constante::alphaDCF*dDCF/4.34);
}

long double Enlace::get_ruido_enlace(int num_slots) {
    assert (num_slots > 0);
    assert (num_slots <= Def::getSE());
    return num_slots*ruido_enlace;
}

double Enlace::get_comprimento() {
    return distancia;
}

long double Enlace::get_perda_enlace() {
    return L_FB;
}
