#ifndef Conexao_H
#define Conexao_H

#include <assert.h>
#include "Def.h"
#include "Route.h"

//////////////////////////////////////////////
class Conexao
{
public:
    Conexao(const Route*, int, int, TIME);
    int getFirstSlot();
    void incFirstSlot(int);
    int getLastSlot();
    void incLastSlot(int);
    const Route* getRoute();
    TIME getTimeDesc();
private:
    const Route* route;
    int Si;
    int Sf;
    TIME tDesc; //Si=Slot inicial; Sf=Slot final
};

#endif
