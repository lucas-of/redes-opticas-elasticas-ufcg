#ifndef EVENT_H
#define EVENT_H

#include "Def.h"
#include "Conexao.h"

enum EventType {UNKNOWN, Req /*Requisição*/, Desc /*Descarte*/, Exp /*Expansão*/, Comp /*Compressão*/};

struct Event {
    TIME time;
    EventType type;
    Event *nextEvent;
    Conexao* conexao;
};

#endif // EVENT_H
