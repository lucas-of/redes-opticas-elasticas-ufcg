#include "Conexao.h"

const int Conexao::numEsquemasDeModulacao = 4;

Conexao::Conexao(Route* r, int si, int sf, TIME tD, EsquemaDeModulacao Esq) : route(r), Si(si), Sf(sf), tDesc(tD), Esquema(Esq) {}

const int Conexao::getFirstSlot() {
    return Si;
}

const int Conexao::getLastSlot() {
    return Sf;
}

const Route* Conexao::getRoute() {
    return route;
}

const TIME Conexao::getTimeDesc() {
    return tDesc;
}

void Conexao::incFirstSlot(int x) {
    assert(Si + x >= 0 && Si + x < Def::getSE());
    Si += x;
}

void Conexao::incLastSlot(int x) {
    assert(Sf + x >= 0 && Sf + x < Def::getSE());
    Sf += x;
}

const int Conexao::getEsquemaDeModulacao() {
    return Esquema;
}
