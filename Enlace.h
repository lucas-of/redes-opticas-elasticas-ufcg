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
        long double get_Nnz();
        void calcula_Nnz();
    private:
        double distancia;
        Node *Origem, *Destino;
        unsigned int num_amplif;
        long double ganho_enlace;
        void calcula_ganho_enlace();
        long double Nnz;//posição do amplificador
};

#endif // ENLACE_H
