#ifndef Conexao_H
#define Conexao_H

#include <assert.h>
#include "Def.h"
#include "Route.h"

class Conexao {
	public:
		Conexao(Route, int*, int*, TIME);
		~Conexao();

		const int getFirstSlot(int i); //retorna primeiro slot da conexao no Node i
		const int getLastSlot(int i); //retorna ultimo slot da conexao
		void setFirstSlot(int i, int si);
		void setLastSlot(int i, int sf);
		const Route* getRoute(); //retorna Rota da conexao (objeto tipo Route)
		const TIME getTimeDesc();
		int getNHops();

		Route route; //Rota
	private:
		int *Si; //Slot Inicial
		int *Sf; //Slot Final
		TIME tDesc;
};

#endif
