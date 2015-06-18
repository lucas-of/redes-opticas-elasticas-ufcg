#ifndef NODE_H
#define NODE_H

#include "Def.h"

class Node {
	public:
		enum TiposDeNo {
			Transparente, Translucido
		};

		Node(int);
		int get_whoami();
		long double get_loss();
		long double get_gain_pot(Def *Config);
		long double get_ruido_pot(Def *Config);
		void set_potenciatx(long double);
		int whoami;
		void set_TipoNo(TiposDeNo);
		TiposDeNo get_TipoNo();

	private:
		long double loss;
		long double gain_pot;
		long double ruido_pot;
		long double potencia_tx;
		void calcula_ganho_pot(Def *Config);
		void calcula_ruido_pot();
		TiposDeNo TipoNo;
};

#endif //NODE_H
