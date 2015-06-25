#ifndef REGENERADORES_H
#define REGENERADORES_H

#include "Route.h"
#include "Event.h"
#include "Main_Auxiliar.h"

class Regeneradores {
public:
    static void RP_NDF(int NumTotalRegeneradores, int NumRegeneradoresPorNo); //Nodal Degree First
    static void RP_CNF(int NumTotalRegeneradores, int NumRegeneradoresPorNo); //Centered Node First
    static void RP_TLP(int NumTotalRegeneradores, int NumRegeneradoresPorNo); //Traffic Load Prediction Based
    static void RP_SQP(int NumTotalRegeneradores, int NumRegeneradoresPorNo, int LNMax); //Signal Quality Prediction Based

    static bool RA_FLR(Route *route, long double BitRate, Def *Config, MAux *Aux); //First Longest Reach Regenerator Assignment
    static bool RA_FNS(Route *route, long double BitRate, Def *Config, MAux *Aux); //First Narrowest Spectrum Regenerator Assignment

    static int SQP_LNMax;
    static long double BR; //BitRate de um Regenerador

private:
    static bool HaEspectroEQualidade(Route* route, long double BitRate, Def *Config);
    static bool HaEspectroEQualidade(Route* route, long double BitRate, Def *Config, EsquemaDeModulacao Esquema);
    static EsquemaDeModulacao MelhorEsquema(Route* route, long double BitRate, Def *Config);

};

#endif // REGENERADORES_H
