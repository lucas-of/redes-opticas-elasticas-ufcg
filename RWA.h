#ifndef RWA_H
#define RWA_H

#include "Heuristics.h"
#include "Main_Auxiliar.h"
#include "Route.h"

class RWA {
	public:
		static bool CheckSlotAvailability(const Route*, const int s); /*Checa se a rota route está disponível para o uso, com o slot s livre em toda a rota*/
		static void Dijkstra(); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede*/
		static void DijkstraAcum(const int orN, const int deN, const int L); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede, considerando número de maneiras de alocar os slots no enlace como o custo do caminho*/
		static void DijkstraFormas(const int orN, const int deN, const int L); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede, considerando número de maneiras de alocar os slots no enlace e número de nós até o destino como o custo do caminho*/
		static void DijkstraSP(); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede*/
		static void DijkstraSPeFormas(const int orN, const int deN, const int L); /*Implementa o algoritmo de roteamento Dijkstra com Shortest Path e Formas.*/
		static void FirstFit(const Route*, const int NslotsReq, int& NslotsUsed, int& si); /*Aloca os slots de acordo com o algoritmo First Fit*/
		static void FirstFitOpt(const Route*, const int NslotsReq, int& NslotsUsed, int& si); /*Aloca os slots de acordo com o algoritmo First Fit, usando as listas otimizadas*/
		static void MostUsed(const Route*, const int NslotsReq, int& NslotsUsed, int& si); /*Aloca os slots, procurando dentre os slots que podem atender a requisição aqueles mais utilizados*/
		static void Random(const Route*, const int NslotsReq, int& NslotsUsed, int& si); /*Dentre os slots que podem atender a solicitação, sorteia um*/
		static int sumOccupation(int s); /*Encontra a ocupação de um certo slot s em todos os enlaces da rede. Para uso em MostUsed()*/
};

#endif // RWA_H
