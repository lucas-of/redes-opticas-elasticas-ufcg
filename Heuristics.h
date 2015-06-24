#ifndef Heuristics_H
#define Heuristics_H

#include <assert.h>
#include "Def.h"
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

class Heuristics {
public:
    static long double calculateCostLink(bool* Disp, int L); //calcula o custo do enlace (depende de quantas maneiras de alocar a requisicao naquele enlace estao disponiveis)
    static long double calculateCostRouting(int hops, bool* Disp, int L); //calcula custa do roteamento inteiro (depende de quantos nos ha na rota, e de quantas maneiras de alocar a requisicao no enlace estao disponiveis)
    static void FFO_convencional(vector<int>**); //organiza o vetor com uma FFO convencional (ordem crescente)
    static void FFO_extremos(vector<int>**); //organiza o vetor com uma FFO de extremos (na forma 1 n 2 n-1 3 [...])
    static void FFO_invertido(vector<int>**); //organiza o vetor com uma FFO invertica (ordem decrescente)
    static void FFO_metrica(vector<int>**); //organiza o vetor com uma FFO, de acordo com alguma metrica
    static void PrintFFlists(vector<int>**); //imprime a lista otimizada
    static int calcNumFormAloc(int L, bool* Disp); //calcula o numero de formas de alocar L slots no enlace, de acordo com sua disponibilidade
private:
    static void TestFFlists(vector<int>**); //testa se a lista de FF e consistente (tem todos os slots, i.e.)
};

#endif
