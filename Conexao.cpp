#include "Conexao.h"

Conexao::Conexao(Route r, int *si, int *sf, TIME tD) : route(r), tDesc(tD) {
    Si = new int[route.getNhops()];
    if ( si != NULL )
        for ( int i = 0; i < route.getNhops(); i++ )
            Si[i] = si[i];
    Sf = new int[route.getNhops()];
    if ( sf != NULL )
        for ( int i = 0; i < route.getNhops(); i++ )
            Sf[i] = sf[i];
}

Conexao::~Conexao() {
    delete[] Si;
    delete[] Sf;
}

const int Conexao::getFirstSlot(int i) {
    assert(i < route.getNhops());
    return Si[i];
}

const int Conexao::getLastSlot(int i) {
    assert(i < route.getNhops());
    return Sf[i];
}

const Route* Conexao::getRoute() {
    return &route;
}

const TIME Conexao::getTimeDesc() {
    return tDesc;
}

int Conexao::getNHops() {
    return route.getNhops();
}

void Conexao::setFirstSlot(int i, int si) {
    assert(i < route.getNhops());
    assert(si < Def::getSE());
    Si[i] = si;
}

void Conexao::setLastSlot(int i, int sf) {
    assert(i < route.getNhops());
    assert(sf < Def::getSE());
    assert(Si[i] <= sf);

    Sf[i] = sf;
}
