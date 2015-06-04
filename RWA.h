#ifndef RWA_H
#define RWA_H

#include "Heuristics.h"
#include "Main_Auxiliar.h"
#include "Route.h"

class RWA {
	public:
		static bool CheckSlotAvailability(const Route*, const int s, Def *Config); /*Checa se a rota route está disponível para o uso, com o slot s livre em toda a rota*/
		static void Dijkstra(MAux *Aux); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede*/
		static void DijkstraAcum(const int orN, const int deN, const int L, Def *Config, MAux *Aux); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede, considerando número de maneiras de alocar os slots no enlace como o custo do caminho*/
		static void DijkstraFormas(const int orN, const int deN, const int L, Def *Config, MAux *Aux); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede, considerando número de maneiras de alocar os slots no enlace e número de nós até o destino como o custo do caminho*/
		static void DijkstraPSR(const int orN, const int deN, const int L, EsquemaDeModulacao Esquema, long double TaxaDeTransmissao, Def *Config, MAux *MainAux); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede, considerando coeficientes do PSR*/
		static void DijkstraSP(MAux *Aux); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede*/
		static void DijkstraSPeFormas(const int orN, const int deN, const int L, long double alfa, Def *Config, MAux *Aux); /*Implementa o algoritmo de roteamento Dijkstra com Shortest Path e Formas.*/
		static void DijkstraRuidoeFormas(const int orN, const int deN, const int L, long double beta, EsquemaDeModulacao Esquema, long double TaxaDeTransmissao, Def *Config, MAux *Aux); /*Implementa o algoritmo de roteamento Dijkstra com Ruído do Enlace normalizado e Formas.*/
		static void FirstFit(const Route*, const int NslotsReq, int& NslotsUsed, int& si, Def *Config); /*Aloca os slots de acordo com o algoritmo First Fit*/
		static void FirstFitOpt(const Route*, const int NslotsReq, int& NslotsUsed, int& si, Def *Config); /*Aloca os slots de acordo com o algoritmo First Fit, usando as listas otimizadas*/
		static void MostUsed(const Route*, const int NslotsReq, int& NslotsUsed, int& si, Def *Config); /*Aloca os slots, procurando dentre os slots que podem atender a requisição aqueles mais utilizados*/
		static void LORModificado(const int orN, const int deN, const int L, Def *Config, MAux *Aux);
		static void OSNRR(MAux *Aux); /*Implementa o algoritmo de roteamento OSNR-R*/
		static void Random(const Route*, const int NslotsReq, int& NslotsUsed, int& si, Def *Config); /*Dentre os slots que podem atender a solicitação, sorteia um*/
		static int sumOccupation(int s, Def *Config); /*Encontra a ocupação de um certo slot s em todos os enlaces da rede. Para uso em MostUsed()*/
	private:
		static void ProcurarRota(Node *orN, Node *Current, Node *deN, std::vector<Node*> *Visitados, long double *BestOSNR, Def *Config, MAux *Aux);
		static bool VerificarInclusao(Node *No, std::vector<Node*> *Visitados);
		static long double *BestOSNR;
};

#endif // RWA_H
