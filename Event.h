#ifndef EVENT_H
#define EVENT_H

#include "Def.h"
#include "Conexao.h"

enum EventType {UNKNOWN, Req /*Requisição*/, Desc /*Descarte*/ };

struct Event {
	TIME time;
	EventType type;
	Event *nextEvent;
	Conexao* conexao;
	EsquemaDeModulacao Esquema;
};

#endif // EVENT_H
