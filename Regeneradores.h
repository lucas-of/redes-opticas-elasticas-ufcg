#ifndef REGENERADORES_H
#define REGENERADORES_H

class Regeneradores {
	public:
		Regeneradores();

		static void RP_NDF(int NumTotalRegeneradores, int NumRegeneradoresPorNo); //Nodal Degree First
		static void RP_CDF(int NumTotalRegeneradores, int NumRegeneradoresPorNo); //Centered Node First
		static void RP_TLP(int NumTotalRegeneradores, int NumRegeneradoresPorNo); //Traffic Load Prediction Based
		static void RP_SQP(int NumTotalRegeneradores, int NumRegeneradoresPorNo); //Signal Quality Prediction Based
};

#endif // REGENERADORES_H
