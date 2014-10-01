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
    ganho_enlace_indiv = pow(General::dB(L_DCF)*General::dB(L_FB),1.0/num_amplif);
}

void Enlace::calcula_ganho_enlace() {
    ganho_enlace = General::dB(L_DCF)*General::dB(L_FB);
}

long double Enlace::get_ganho_enlace() {
    return ganho_enlace;
}

void Enlace::calcula_ruido_enlace() {
    if (num_amplif == 0) ruido_enlace = 0;
    else {
        ruido_enlace = Constante::h*Constante::c*Def::get_Bslot()*General::dB(Def::get_Famp())/(2.0 * Def::getlambda());
        long double sum = 0;
        for (int k = 1; k<= num_amplif ; k++) {
            sum += pow(ganho_enlace_indiv,num_amplif+1-k)/pow(L_FB*L_DCF, (num_amplif+1.0-k)/(1.0*(num_amplif + 1)));
        }
        ruido_enlace *= sum;
    }
}

void Enlace::calcula_perdas() {
    L_FB = exp(Constante::alphaFB*distancia/4.34);
    L_DCF = exp(Constante::alphaDCF*distancia/4.34);
}

long double Enlace::get_ruido_enlace(int num_slots) {
    assert (num_slots > 0);
    assert (num_slots <= Def::getSE());
    return num_slots*ruido_enlace;
}
