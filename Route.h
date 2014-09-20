#ifndef Route_H
#define Route_H

#include <assert.h>
#include <iostream>
#include <vector>
#include "Node.h"

class Route {
    public:
        Route (std::vector<Node*> &);
        int getDeN() const; //retorna ultimo no da rota
        unsigned int getNhops() const; //retorna numero de nos ate o destino
        int getNode(int) const; //retorna o no na posicao i da rota
        int getOrN() const; //retorna primeiro no da rota
        void print() const; //imprime rota
    private:
        std::vector<int> Path;
};

#endif
