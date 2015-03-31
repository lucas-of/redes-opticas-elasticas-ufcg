#ifndef Conexao_H
#define Conexao_H

#include <assert.h>
#include "Def.h"
#include "Route.h"

class Conexao {
	public:
		Conexao(Route, int, int, TIME);

		const int getFirstSlot(); //retorna primeiro slot da conexao
		const int getLastSlot(); //retorna ultimo slot da conexao
		const Route* getRoute(); //retorna Rota da conexao (objeto tipo Route)
		const TIME getTimeDesc();
		void incFirstSlot(int); //incrementa primeiro slot por um inteiro n
		void incLastSlot(int); //incrementa ultimo slot por um inteiro n

	private:
		int Si; //Slot Inicial
		int Sf; //Slot Final
		Route route; //Rota
		TIME tDesc;
};

#endif
