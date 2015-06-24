#include "Regeneradores.h"
#include "Def.h"
#include "Main_Auxiliar.h"
#include "RWA.h"
#include "Event.h"

long double AvaliarOSNR(const Route *Rota, Def *Config);
long double Simula_Rede(Def *Config, MAux *Aux);
int SlotsReq(long double BitRate, EsquemaDeModulacao Esquema);
void TryToConnect(const Route* route, const int NslotsReq, int& NslotsUsed, int& si, Def *Config);

int Regeneradores::SQP_LNMax;
long double Regeneradores::BR = 100E9;

void Regeneradores::RP_NDF(int NumTotalRegeneradores, int NumRegeneradoresPorNo) {
    assert(NumTotalRegeneradores % NumRegeneradoresPorNo == 0);
    int NumNoTransparentes = NumTotalRegeneradores / NumRegeneradoresPorNo;
    assert(NumNoTransparentes <= Def::Nnodes);

    vector<int> GrauNo;
    for ( unsigned i = 0; i < Def::GrauNo.size(); i++ )
        GrauNo.push_back(Def::GrauNo.at(i));
    for ( int i = 0; i < NumNoTransparentes; i++ ) {
        int MaiorGrau = GrauNo.at(0);
        for ( unsigned j = 0; j < GrauNo.size(); j++ )
            if ( MaiorGrau < GrauNo.at(j) ) MaiorGrau = GrauNo.at(j);

        vector<int> NosMaximos;
        for ( unsigned j = 0; j < GrauNo.size(); j++ )
            if ( (GrauNo.at(j) == MaiorGrau) && (MAux::Rede.at(j).get_TipoNo() == Node::Transparente) )
                NosMaximos.push_back(j);

        int NoEscolhido = NosMaximos.at(floor(General::uniforme(0, NosMaximos.size())));
        MAux::Rede.at(NoEscolhido).set_TipoNo(Node::Translucido);
        MAux::Rede.at(NoEscolhido).set_NumRegeneradores(NumRegeneradoresPorNo);

        GrauNo.at(NoEscolhido) = -1;
    }
}

void Regeneradores::RP_CNF(int NumTotalRegeneradores, int NumRegeneradoresPorNo) {
    if ( NumTotalRegeneradores != 0 ) assert(NumTotalRegeneradores % NumRegeneradoresPorNo == 0);
    int NumNoTransparentes = NumTotalRegeneradores / NumRegeneradoresPorNo;
    assert(NumNoTransparentes <= Def::Nnodes);

    vector <int> CentralidadeNo(Def::Nnodes);
    for ( unsigned i = 0; i < CentralidadeNo.size(); i++ ) CentralidadeNo.at(i) = 0;

    MAux MainAux;
    RWA::DijkstraSP(&MainAux);

    for ( int orN = 0; orN < Def::getNnodes(); orN++ ) {
        for ( int deN = 0; deN < Def::getNnodes(); deN++ ) {
            if ( orN == deN ) continue;
            for ( int No = 0; No <= MainAux.AllRoutes[orN * Def::Nnodes + deN].at(0)->getNhops(); No++ )
                CentralidadeNo.at(MainAux.AllRoutes[orN * Def::Nnodes + deN].at(0)->getNode(No))++;
        }
    }

    for ( int i = 0; i < NumNoTransparentes; i++ ) {
        int MaiorGrau = CentralidadeNo.at(0);
        for ( unsigned j = 0; j < CentralidadeNo.size(); j++ )
            if ( (MaiorGrau < CentralidadeNo.at(j)) && (MAux::Rede.at(j).get_TipoNo() == Node::Transparente) )
                MaiorGrau = CentralidadeNo.at(j);

        vector<int> NosMaximos;
        for ( unsigned j = 0; j < CentralidadeNo.size(); j++ )
            if ( CentralidadeNo.at(j) == MaiorGrau ) NosMaximos.push_back(j);

        int NoEscolhido = NosMaximos.at(floor(General::uniforme(0, NosMaximos.size())));
        MAux::Rede.at(NoEscolhido).set_TipoNo(Node::Translucido);
        MAux::Rede.at(NoEscolhido).set_NumRegeneradores(NumRegeneradoresPorNo);
        CentralidadeNo.at(NoEscolhido) = -1;
    }
}

void Regeneradores::RP_TLP(int NumTotalRegeneradores, int NumRegeneradoresPorNo) {
    assert(NumTotalRegeneradores % NumRegeneradoresPorNo == 0);
    int NumNoTransparentes = NumTotalRegeneradores / NumRegeneradoresPorNo;
    assert(NumNoTransparentes <= Def::Nnodes);

    MAux::FlagRP_TLP = true;
    MAux *MainAux = new MAux();
    Def *Config = new Def(NULL);
    MainAux->laNet = MainAux->LaNetMin;
    Simula_Rede(Config, MainAux);

    for ( int i = 0; i < NumNoTransparentes; i++ ) {
        int MaiorGrau = MainAux->RP_TLP_NodeUsage[0];
        for ( int j = 0; j < Def::Nnodes; j++ )
            if ( MaiorGrau < MainAux->RP_TLP_NodeUsage[j] ) MaiorGrau = MainAux->RP_TLP_NodeUsage[j];

        vector<int> NosMaximos;
        for ( int j = 0; j < Def::Nnodes; j++ )
            if ( (MainAux->RP_TLP_NodeUsage[j] == MaiorGrau) && (MAux::Rede.at(j).get_TipoNo() == Node::Transparente) )
                NosMaximos.push_back(j);

        int NoEscolhido = NosMaximos.at(floor(General::uniforme(0, NosMaximos.size())));
        MAux::Rede.at(NoEscolhido).set_TipoNo(Node::Translucido);
        MAux::Rede.at(NoEscolhido).set_NumRegeneradores(NumRegeneradoresPorNo);
        MainAux->RP_TLP_NodeUsage[NoEscolhido] = -1;
    }

    MAux::FlagRP_TLP = false;
    delete MainAux;
    delete Config;
}

void Regeneradores::RP_SQP(int NumTotalRegeneradores, int NumRegeneradoresPorNo, int LNMax) {
    assert(NumTotalRegeneradores % NumRegeneradoresPorNo == 0);
    int NumNoTransparentes = NumTotalRegeneradores / NumRegeneradoresPorNo;
    assert(NumNoTransparentes <= Def::Nnodes);
    assert(LNMax > 0);

    SQP_LNMax = LNMax;
    MAux::FlagRP_SQP = true;
    MAux *MainAux = new MAux();
    Def *Config = new Def(NULL);
    MainAux->laNet = MainAux->LaNetMin;
    Simula_Rede(Config, MainAux);

    for ( int i = 0; i < NumNoTransparentes; i++ ) {
        int MaiorGrau = MainAux->RP_SQP_NodeUsage[0];
        for ( int j = 0; j < Def::Nnodes; j++ )
            if ( MaiorGrau < MainAux->RP_SQP_NodeUsage[j] ) MaiorGrau = MainAux->RP_SQP_NodeUsage[j];

        vector<int> NosMaximos;
        for ( int j = 0; j < Def::Nnodes; j++ )
            if ( (MainAux->RP_SQP_NodeUsage[j] == MaiorGrau) && (MAux::Rede.at(j).get_TipoNo() == Node::Transparente) )
                NosMaximos.push_back(j);

        int NoEscolhido = NosMaximos.at(floor(General::uniforme(0, NosMaximos.size())));
        MAux::Rede.at(NoEscolhido).set_TipoNo(Node::Translucido);
        MAux::Rede.at(NoEscolhido).set_NumRegeneradores(NumRegeneradoresPorNo);
        MainAux->RP_SQP_NodeUsage[NoEscolhido] = -1;
    }

    MAux::FlagRP_SQP = false;
    delete MainAux;
    delete Config;
}

bool Regeneradores::RA_FLR(Route *route, long double BitRate, Def *Config, Event *evt) {
    assert(BitRate > 0);

    evt->Esquema = _64QAM; //Esse algoritmo sempre tenta criar as chamadas com 64QAM.

    int RegeneneradoresNecessarios = ceil(BitRate / BR);
    int NoS, NoX;
    int r = 0;
    int s, x;
    int L_or, L_de;
    int NslotsUsed;
    int NslotsReq = SlotsReq(BitRate, evt->Esquema);
    int SI; //SlotInicial

    vector<int> SegmentosTransparentes;
    int numSegmentosTransparentes = 0;
    vector<EsquemaDeModulacao> Esquemas;
    SegmentosTransparentes.push_back(route->getNode(0));

    for ( s = 0; s <= route->getNhops(); s++ ) {
        NoS = route->getNode(s);
        for ( x = s + 1; x <= route->getNhops(); x++ ) {
            NoX = route->getNode(x); //descobre o x-esimo no da rota
            if ( (MAux::Rede.at(NoX).get_NumRegeneradoresDisponiveis() >= RegeneneradoresNecessarios) || (x == route->getNhops()) ) {
                Route rotaQuebrada = *route->breakRoute(NoS, NoX);
                NslotsUsed = 0;
                SI = -1;
                TryToConnect(&rotaQuebrada, NslotsReq, NslotsUsed, SI, Config);
                long double OSNR = AvaliarOSNR(&rotaQuebrada, Config);
                if ( (SI != -1) && (OSNR >= Def::getlimiarOSNR(evt->Esquema, BitRate)) ) {
                    if ( x == route->getNhops() ) { //destino
                        SegmentosTransparentes.push_back(NoX);
                        for ( int i = 0; i < numSegmentosTransparentes; i++ ) {
                            Route rotaQuebrada = *route->breakRoute(SegmentosTransparentes.at(i), SegmentosTransparentes.at(i + 1));
                            TryToConnect(&rotaQuebrada, NslotsReq, NslotsUsed, SI, Config);

                            assert(SI != -1);
                            assert(NslotsReq == NslotsUsed);

                            evt->conexao->setFirstSlot(i, SI);
                            evt->conexao->setLastSlot(i, SI + NslotsReq - 1);

                            for ( unsigned c = 0; c < rotaQuebrada.getNhops(); c++ ) {
                                L_or = rotaQuebrada.getNode(c);
                                L_de = rotaQuebrada.getNode(c + 1);
                                for ( int s = evt->conexao->getFirstSlot(i); s < evt->conexao->getLastSlot(i); s++ ) {
                                    assert(Config->Topology_S[s * Def::Nnodes * Def::Nnodes + L_or * Def::Nnodes + L_de] == false);
                                    Config->Topology_S[s * Def::Nnodes * Def::Nnodes + L_or * Def::Nnodes + L_de] = true;
                                    //Os slots sao marcados como ocupados
                                }
                            }

                            int NumReg = MAux::Rede.at(SegmentosTransparentes.at(i)).solicitar_regeneradores(BitRate);
                            assert(NumReg != 0); //Conseguiu solicitar os regeneradores
                            evt->RegeneradoresUtilizados[ SegmentosTransparentes.at(i) ] = NumReg;
                            evt->TotalRegeneradoresUtilizados += NumReg;
                        }
                        return true; //chamada aceita
                    } else {
                        r = x; //atualiza ponto de regeneração
                    }
                }
            } else {
                if ( r != s ) {
                    numSegmentosTransparentes++;
                    SegmentosTransparentes.push_back(route->getNode(r));
                    Esquemas.push_back(_64QAM);
                    s = r; //atualiza fonte
                    x = r; //atualiza ponto de teste
                } else {
                    return false; //chamada bloqueada.
                }
            }
        }
    }
    return false;
}

bool Regeneradores::RA_FNS(Route *route, long double BitRate, Def *Config, Event *evt) {
    assert(BitRate > 0);

    EsquemaDeModulacao EscolhaEsquemas[numEsquemasDeModulacao] = {_64QAM, _16QAM, _4QAM}; //Esse algoritmo tenta implementar as chamadas usando esses Esquemas, nessa ordem.

    int RegeneneradoresNecessarios = ceil(BitRate / BR);
    int NoS, NoX;
    int r = 0;
    int s, x;
    int L_or, L_de;
    int NslotsUsed;
    int NslotsReq;
    int SI; //SlotInicial
    int m = 0; //Primeiro Esquema

    vector<int> SegmentosTransparentes;
    int numSegmentosTransparentes = 0;
    vector<EsquemaDeModulacao> EsquemasUtilizados;
    SegmentosTransparentes.push_back(route->getNode(0));

    for ( s = 0; s < route->getNhops(); s++ ) {
        NoS = route->getNode(s);
        for ( x = s + 1; x <= route->getNhops(); x++ ) {
            NoX = route->getNode(x); //descobre o x-esimo no da rota
            if ( (MAux::Rede.at(NoX).get_NumRegeneradoresDisponiveis() >= RegeneneradoresNecessarios) || (x == route->getNhops()) ) {
                Route rotaQuebrada = *route->breakRoute(NoS, NoX);
                NslotsUsed = 0;
                SI = -1;
                NslotsReq = SlotsReq(BitRate, EscolhaEsquemas[m]);
                TryToConnect(&rotaQuebrada, NslotsReq, NslotsUsed, SI, Config);
                long double OSNR = AvaliarOSNR(&rotaQuebrada, Config);
                if ( (SI != -1) && (OSNR >= Def::getlimiarOSNR(EscolhaEsquemas[m], BitRate)) ) { //Há qualidade
                    if ( x == route->getNhops() ) { //destino
                        SegmentosTransparentes.push_back(NoX);
                        for ( int i = 0; i < numSegmentosTransparentes; i++ ) {
                            Route rotaQuebrada = *route->breakRoute(SegmentosTransparentes.at(i), SegmentosTransparentes.at(i + 1));
                            NslotsReq = SlotsReq(BitRate, EsquemasUtilizados.at(i));
                            TryToConnect(&rotaQuebrada, NslotsReq, NslotsUsed, SI, Config);

                            assert(SI != -1);
                            assert(NslotsReq == NslotsUsed);

                            evt->conexao->setFirstSlot(i, SI);
                            evt->conexao->setLastSlot(i, SI + NslotsReq - 1);

                            for ( unsigned c = 0; c < rotaQuebrada.getNhops(); c++ ) {
                                L_or = rotaQuebrada.getNode(c);
                                L_de = rotaQuebrada.getNode(c + 1);
                                for ( int s = evt->conexao->getFirstSlot(i); s < evt->conexao->getLastSlot(i); s++ ) {
                                    assert(Config->Topology_S[s * Def::Nnodes * Def::Nnodes + L_or * Def::Nnodes + L_de] == false);
                                    Config->Topology_S[s * Def::Nnodes * Def::Nnodes + L_or * Def::Nnodes + L_de] = true;
                                    //Os slots sao marcados como ocupados
                                }
                            }

                            int NumReg = MAux::Rede.at(SegmentosTransparentes.at(i)).solicitar_regeneradores(BitRate);
                            assert(NumReg != 0); //Conseguiu solicitar os regeneradores

                            evt->RegeneradoresUtilizados[ SegmentosTransparentes.at(i) ] = NumReg;
                        }
                        return true;
                    } else {
                        if ( m != 0 ) {
                            numSegmentosTransparentes++;
                            SegmentosTransparentes.push_back(route->getNode(r));
                            EsquemasUtilizados.push_back(EscolhaEsquemas[m]);
                            r = x;
                            s = x;
                            m = 0;
                        } else {
                            r = x; //atualiza ponto de regeneração
                        }
                    }
                }
            } else {
                if ( r != s ) {
                    numSegmentosTransparentes++;
                    SegmentosTransparentes.push_back(route->getNode(r));
                    EsquemasUtilizados.push_back(EscolhaEsquemas[m]);
                    s = r; //atualiza fonte
                    x = r; //atualiza ponto de teste
                } else {
                    x--;
                    m++;
                    if ( m == numEsquemasDeModulacao )
                        return false; //chamada bloqueada.
                }
            }
        }
    }
    return false;
}
