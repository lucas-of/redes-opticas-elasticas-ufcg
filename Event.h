#ifndef EVENT_H
#define EVENT_H

#include "Def.h"
#include "Conexao.h"

enum EventType {
    UNKNOWN, Req /*Requisição*/, Desc /*Descarte*/
};

struct Event {
    TIME time;
    EventType type;
    Event *nextEvent;
    Conexao* conexao;
    EsquemaDeModulacao Esquema;
    int *RegeneradoresUtilizados;
    int TotalRegeneradoresUtilizados = 0;

    Event() {
        RegeneradoresUtilizados = new int[Def::Nnodes];
        for (int i = 0; i < Def::Nnodes; i++) RegeneradoresUtilizados[i] = 0;
    };

    ~Event() {
        delete[] RegeneradoresUtilizados;
    }
};

#endif // EVENT_H
