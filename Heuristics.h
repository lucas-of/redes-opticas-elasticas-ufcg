#ifndef Heuristics_H
#define Heuristics_H

#include <assert.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
using namespace std;

#include "Def.h"

/////////////////////////////////////////////////////////////
class Heuristics
{
public:
    static void FFO_convencional(vector<int>**);
    static void FFO_extremos(vector<int>**);
    static void FFO_invertido(vector<int>**);
    static void FFO_metrica(vector<int>**);
    static void TestFFlists(vector<int>**);
    static void PrintFFlists(vector<int>**);
    //
    static long double calculateCostLink(bool* Disp, int L);
    static long double calculateCostRouting(int hops, bool* Disp, int L);
private:
    static int calcNumFormAloc(int L, bool* Disp);
    static int calcNumAloc(int L, bool* Disp);
};

#endif
