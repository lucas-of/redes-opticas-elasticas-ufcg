#ifndef ENLACE_H
#define ENLACE_H

#include "Node.h"

class Enlace {
	public:
		Enlace(Node *NOrig, Node *NDest, double dist);
		double get_comprimento(void);
		long double get_peso(void);
		Node* get_NodeOrigem(void);
		Node* get_NodeDestino(void);
		long double get_ganho_enlace();
		long double get_ganho_preamplif();
		long double get_ruido_preamplif();
		long double get_perda_enlace();
		long double get_ruido_enlace(int num_slots);
		void recalcular();
		void recalcular_peso(long double *Coef);
	private:
		double distancia;
		long double peso;
		Node *Origem, *Destino;
		int num_amplif;
		long double ganho_enlace;
		long double ganho_enlace_indiv;
		long double ruido_enlace;
		long double ganho_preamplif;
		long double ruido_preamplif;
		long double L_FB;
		long double L_DCF;
		long double *Coeficientes;
		void calcula_perdas();
		void calcula_ganho_enlace();
		void calcula_ruido_enlace();
		void calcula_num_amplificadores();
		void calcula_preamplif();
};

#endif // ENLACE_H
