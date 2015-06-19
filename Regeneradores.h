#ifndef REGENERADORES_H
#define REGENERADORES_H

class Regeneradores {
	public:
		Regeneradores();

		static void RP_NDF(int NumTotalRegeneradores, int NumRegeneradoresPorNo); //Nodal Degree First
		static void RP_CNF(int NumTotalRegeneradores, int NumRegeneradoresPorNo); //Centered Node First
		static void RP_TLP(int NumTotalRegeneradores, int NumRegeneradoresPorNo); //Traffic Load Prediction Based
		static void RP_SQP(int NumTotalRegeneradores, int NumRegeneradoresPorNo, int LNMax); //Signal Quality Prediction Based

		static int SQP_LNMax;

};

#endif // REGENERADORES_H
