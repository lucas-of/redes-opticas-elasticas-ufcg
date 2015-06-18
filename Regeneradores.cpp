#include "Regeneradores.h"
#include "Def.h"
#include "Main_Auxiliar.h"

Regeneradores::Regeneradores() {};

void Regeneradores::RP_NDF(int NumTotalRegeneradores, int NumRegeneradoresPorNo) {
	assert (NumTotalRegeneradores % NumRegeneradoresPorNo == 0);
	int NumNoTransparentes = NumTotalRegeneradores/NumRegeneradoresPorNo;
	assert(NumNoTransparentes <= Def::Nnodes);

	vector<int> GrauNo = Def::GrauNo;
	for (int i = 0; i < NumNoTransparentes; i++) {
		int MaiorGrau = GrauNo.at(0);
		for (int j = 0; j < GrauNo.size(); j++)
			if (MaiorGrau < GrauNo.at(j)) MaiorGrau = GrauNo.at(j);

		vector<int> NosMaximos;
		for (int j = 0; j < GrauNo.size(); j++)
			if (GrauNo.at(j) == MaiorGrau) NosMaximos.push_back(j);

		int NoEscolhido = ceil( General::uniforme(0, NosMaximos.size()) );
		MAux::Rede.at(NoEscolhido).set_TipoNo( Node::Translucido );
	}
}
