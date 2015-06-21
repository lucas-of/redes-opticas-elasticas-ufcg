#ifndef REGENERADORES_H
#define REGENERADORES_H

#include "Route.h"

class Regeneradores {
	public:
		Regeneradores();

		static void RP_NDF(int NumTotalRegeneradores, int NumRegeneradoresPorNo); //Nodal Degree First
		static void RP_CNF(int NumTotalRegeneradores, int NumRegeneradoresPorNo); //Centered Node First
		static void RP_TLP(int NumTotalRegeneradores, int NumRegeneradoresPorNo); //Traffic Load Prediction Based
		static void RP_SQP(int NumTotalRegeneradores, int NumRegeneradoresPorNo, int LNMax); //Signal Quality Prediction Based

		static bool RA_FLR(Route route, int NslotsReq, int BitRate, int *si, Def *Config); //First Longest Reach Regenerator Assignment

		static int SQP_LNMax;
		static int BR; //BitRate de um Regenerador

};

#endif // REGENERADORES_H
