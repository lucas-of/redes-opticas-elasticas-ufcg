#include "Conexao.h"

Conexao::Conexao(Route r, int *si, int *sf, TIME tD) : route(r), tDesc(tD) {
	Si = new int[route.getNhops()+1];
	for (int i = 0; i <= route.getNhops(); i++)
		Si[i] = si[i];
	Sf = new int[route.getNhops()+1];
	for (int i = 0; i <= route.getNhops(); i++)
		Sf[i] = sf[i];
}

Conexao::~Conexao(){
	delete[] Si;
	delete[] Sf;
}

const int Conexao::getFirstSlot(int i) {
	return Si[i];
}

const int Conexao::getLastSlot(int i) {
	return Sf[i];
}

const Route* Conexao::getRoute() {
	return &route;
}

const TIME Conexao::getTimeDesc() {
	return tDesc;
}

int Conexao::getNHops() {
	return route.getNhops();
}
