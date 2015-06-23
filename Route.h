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
		int getNhops() const; //retorna numero de nos ate o destino
		int getNode(unsigned int) const; //retorna o no na posicao i da rota
		int getOrN() const; //retorna primeiro no da rota
		void print() const; //imprime rota
		Route* breakRoute( int OrN, int DeN );
		std::vector<Node*>* get_Nodes();
	private:
		std::vector<int> Path;
		std::vector<Node*> Nodes;
};

#endif
