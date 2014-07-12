#include "Conexao.h"

////////////////////////////////////////////////////////////////
Conexao::Conexao(const Route* r, int si, int sf, TIME tD)
    :route(r), Si(si), Sf(sf), tDesc(tD)
{

}

////////////////////////////////////////////////////////////////
int Conexao::getFirstSlot()
{
    return Si;
}

// ------------------------------------------------- //
void Conexao::incFirstSlot(int x)
{
    assert(Si + x >= 0 && Si + x < Def::getSE());
}

// ------------------------------------------------- //
int Conexao::getLastSlot()
{
    return Sf;
}

// ------------------------------------------------- //
void Conexao::incLastSlot(int x)
{
    assert(Sf + x >= 0 && Sf + x < Def::getSE());
}

// ------------------------------------------------- //
const Route* Conexao::getRoute()
{
    return route;
}

// ------------------------------------------------- //
TIME Conexao::getTimeDesc()
{
    return tDesc;
}
