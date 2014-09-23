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
    private:
        double distancia;
        Node *Origem, *Destino;
        unsigned int num_amplif;
        long double ganho_enlace;
        void calcula_ganho_enlace();
        void calcula_num_amplificadores(double dist);
};

#endif // ENLACE_H
