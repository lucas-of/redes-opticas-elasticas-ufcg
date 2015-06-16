#ifndef NODE_H
#define NODE_H

#include "Def.h"

class Node {
	private:
		long double loss;
		long double gain_pot;
		long double ruido_pot;
		long double potencia_tx;
		void calcula_ganho_pot(Def *Config);

		int cont;
		int reg;
		int transp;
		int ur;
		void calcula_ruido_pot();
	public:
		Node(int);
		int get_whoami();
		long double get_loss();
		long double get_gain_pot(Def *Config);
		long double get_ruido_pot(Def *Config);
		void set_potenciatx(long double);
		int whoami;
		void set_regenerador(int x);
		int get_regenerador();
		int available_regenerators(int c);
		void set_n_used_regenerators(int Br);
		int get_n_used_regenerators();
		void set_transp_seg(int, int, int);
		int get_transp_seg();
};

#endif //NODE_H
