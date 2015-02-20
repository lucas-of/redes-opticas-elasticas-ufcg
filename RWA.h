#ifndef RWA_H
#define RWA_H

#include "Heuristics.h"
#include "Main_Auxiliar.h"

bool CheckSlotAvailability(const Route*, const int s); /*Checa se a rota route está disponível para o uso, com o slot s livre em toda a rota*/
void Dijkstra(); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede*/
void DijkstraAcum(const int orN, const int deN, const int L); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede, considerando número de maneiras de alocar os slots no enlace como o custo do caminho*/
void DijkstraFormas(const int orN, const int deN, const int L); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede, considerando número de maneiras de alocar os slots no enlace e número de nós até o destino como o custo do caminho*/
void DijkstraSP(); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede*/
void DijkstraSPeFormas(const int orN, const int deN, const int L); /*Implementa o algoritmo de roteamento Dijkstra com Shortest Path e Formas.*/
void FirstFit(const Route*, const int NslotsReq, int& NslotsUsed, int& si); /*Aloca os slots de acordo com o algoritmo First Fit*/
void FirstFitOpt(const Route*, const int NslotsReq, int& NslotsUsed, int& si); /*Aloca os slots de acordo com o algoritmo First Fit, usando as listas otimizadas*/
void MostUsed(const Route*, const int NslotsReq, int& NslotsUsed, int& si); /*Aloca os slots, procurando dentre os slots que podem atender a requisição aqueles mais utilizados*/
void Random(const Route*, const int NslotsReq, int& NslotsUsed, int& si); /*Dentre os slots que podem atender a solicitação, sorteia um*/
int sumOccupation(int s); /*Encontra a ocupação de um certo slot s em todos os enlaces da rede. Para uso em MostUsed()*/

#endif // RWA_H
