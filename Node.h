#ifndef NODE_H
#define NODE_H

class Node {
	private:
		long double loss;
		long double gain_pot;
		long double ruido_pot;
		long double potencia_tx;
        int cont;
        int reg;
        int transp;
        int ur;
        //int tamanho;
        //int segmento[tamanho];
		void calcula_ganho_pot();
		void calcula_ruido_pot();
	public:
		Node(int);
		int get_whoami();
		long double get_loss();
		long double get_gain_pot();
		long double get_ruido_pot(int);
		void set_potenciatx(long double);
		int whoami;
        void set_regenerador(int x);
        int get_regenerador();
        int available_regenerators(int c);
        void set_n_used_regenerators(int Br);
        int get_n_used_regenerators();
        void set_transp_seg(int, int, int);
};

#endif //NODE_H
