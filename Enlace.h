#ifndef ENLACE_H
#define ENLACE_H

#include "Node.h"

class Enlace {
    public:
        Enlace(Node *NOrig, Node *NDest, double dist);
        double get_comprimento(void);
        Node* get_NodeOrigem(void);
        Node* get_NodeDestino(void);
        long double get_ganho_enlace();
        long double get_ruido_enlace();
    private:
        double distancia;
        Node *Origem, *Destino;
        int num_amplif;
        long double ganho_enlace;
        long double ganho_enlace_indiv;
        long double ruido_enlace;
        void calcula_ganho_enlace();
        void calcula_ganho_enlace_indiv();
        void calcula_ruido_enlace();
        void calcula_num_amplificadores(double dist);
};

#endif // ENLACE_H
