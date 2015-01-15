#include "Def.h"
#include "Main_Auxiliar.h"

long double ProbBloqueio() {
    return Def::numReq_Bloq/Def::numReq;
}

long double ProbAceitacao() {
    return 1.0 - ProbBloqueio();
}

void ProbBloqueioTaxa() {
    for (int i = 0; i < Def::get_numPossiveisTaxas(); i++ ) {
        ResulProbBloqTaxa << Def::PossiveisTaxas[i] << " " << Def::numReqBloq_Taxa[i]/Def::numReq_Taxa[i] << endl;
        cout << Def::PossiveisTaxas[i] << "Gbps\tProb Bloq = " << Def::numReqBloq_Taxa[i]/Def::numReq_Taxa[i] << endl;
    }
}

void ProbAceitacaoTaxa() {
    for (int i = 0; i < Def::get_numPossiveisTaxas(); i++ ) {
        ResulProbAceitTaxa << Def::PossiveisTaxas[i] << " " << 1.0 - Def::numReqBloq_Taxa[i]/Def::numReq_Taxa[i] << endl;
    }
}
