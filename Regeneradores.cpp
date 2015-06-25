#include "Regeneradores.h"
#include "Def.h"
#include "Main_Auxiliar.h"
#include "RWA.h"
#include "Event.h"

long double AvaliarOSNR(const Route *Rota, Def *Config);
Event* InserirConexao(Route* route, int si, int NslotsUsed, long double Tempo, MAux *Aux, Def* Config);
long double Simula_Rede(Def *Config, MAux *Aux);
int SlotsReq(long double BitRate, EsquemaDeModulacao Esquema);
void TryToConnect(const Route* route, const int NslotsReq, int& NslotsUsed, int& si, Def *Config);

int Regeneradores::SQP_LNMax;
long double Regeneradores::BR = 100E9;

void Regeneradores::RP_NDF(int NumTotalRegeneradores, int NumRegeneradoresPorNo) {
    assert(NumTotalRegeneradores % NumRegeneradoresPorNo == 0);
    int NumNoTransparentes = NumTotalRegeneradores / NumRegeneradoresPorNo;
    int NoEscolhido;
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

        NoEscolhido = NosMaximos.at(floor(General::uniforme(0, NosMaximos.size())));
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

bool Regeneradores::RA_FLR(Route *route, long double BitRate, Def *Config, MAux *Aux) {
    assert(BitRate > 0);
    assert(HaEspectroEQualidade(route, BitRate, Config) == false); //RA só pode ser chamado em rotas sem qualidade/espectro

    Event *evt;

    int RegeneneradoresNecessarios = ceil(BitRate / BR);
    int NoS, NoX;
    int r = 0;
    int s, x;
    int L_or, L_de;
    int NslotsUsed;
    int NslotsReq;
    int SI = -1; //SlotInicial

    vector<int> SegmentosTransparentes;
    int numSegmentosTransparentes = 0;
    SegmentosTransparentes.push_back(route->getNode(0));

    for ( s = 0; s <= route->getNhops(); s++ ) {
        NoS = route->getNode(s);
        for ( x = s + 1; x <= route->getNhops(); x++ ) {
            NoX = route->getNode(x); //descobre o x-esimo no da rota
            if ( (MAux::Rede.at(NoX).get_NumRegeneradoresDisponiveis() >= RegeneneradoresNecessarios) || (x == route->getNhops()) ) {
                Route rotaQuebrada = *route->breakRoute(NoS, NoX);
                if ( HaEspectroEQualidade(&rotaQuebrada, BitRate, Config) ) {
                    if ( x == route->getNhops() ) { //destino
                        SegmentosTransparentes.push_back(NoX);

                        //Insere conexão na rede
                        long double Tempo = General::exponential(MAux::mu);
                        evt = InserirConexao(route, 0, 1, Tempo, Aux, Config);

                        int hop = 0;
                        for ( int i = 0; i <= numSegmentosTransparentes; i++ ) {
                            SI = -1;
                            NslotsUsed = 0;
                            Route Segmento = *route->breakRoute(SegmentosTransparentes.at(i), SegmentosTransparentes.at(i + 1));
                            NslotsReq = SlotsReq(BitRate, MelhorEsquema(&Segmento, BitRate, Config));
                            TryToConnect(&Segmento, NslotsReq, NslotsUsed, SI, Config);

                            assert(SI != -1);
                            assert(NslotsReq == NslotsUsed);

                            for ( int c = 0; c < Segmento.getNhops(); c++ ) {
                                evt->conexao->setFirstSlot(hop, SI);
                                evt->conexao->setLastSlot(hop, SI + NslotsReq - 1);
                                L_or = Segmento.getNode(c);
                                L_de = Segmento.getNode(c + 1);
                                for ( int s = evt->conexao->getFirstSlot(hop); s <= evt->conexao->getLastSlot(hop); s++ ) {
                                    assert(Config->Topology_S[s * Def::Nnodes * Def::Nnodes + L_or * Def::Nnodes + L_de] == false);
                                    Config->Topology_S[s * Def::Nnodes * Def::Nnodes + L_or * Def::Nnodes + L_de] = true;
                                    //Os slots sao marcados como ocupados
                                }
                                hop++;
                            }

                            if ( i != 0 ) {
                                int NumReg = MAux::Rede.at(SegmentosTransparentes.at(i)).solicitar_regeneradores(BitRate);
                                assert(NumReg != 0); //Conseguiu solicitar os regeneradores
                                evt->RegeneradoresUtilizados[ SegmentosTransparentes.at(i) ] = NumReg;
                                evt->TotalRegeneradoresUtilizados += NumReg;
                            }
                        }
                        return true; //chamada aceita
                    } else {
                        r = x; //atualiza ponto de regeneração
                    }
                } else {
                    if ( r != s ) {
                        numSegmentosTransparentes += 1;
                        SegmentosTransparentes.push_back(route->getNode(r));
                        s = r; //atualiza fonte
                        NoS = route->getNode(s);
                        x = r; //atualiza ponto de teste
                        NoX = route->getNode(x);
                    } else {
                        return false; //chamada bloqueada.
                    }
                }
            }
        }
    }
    return false;
}

bool Regeneradores::RA_FNS(Route *route, long double BitRate, Def *Config, MAux *Aux) {
    assert(BitRate > 0);
    assert(HaEspectroEQualidade(route, BitRate, Config) == false); //RA só pode ser chamado em rotas sem qualidade/espectro

    Event *evt;

    EsquemaDeModulacao EscolhaEsquemas[] = {_64QAM, _16QAM, _4QAM}; //Esse algoritmo tenta implementar as chamadas usando esses Esquemas, nessa ordem.

    int RegeneneradoresNecessarios = ceil(BitRate / BR);
    int NoS, NoX;
    int r = 0;
    int s, x;
    int L_or, L_de;
    int NslotsUsed;
    int NslotsReq;
    int SI = -1; //SlotInicial
    int m = 0; //Primeiro Esquema

    vector<int> SegmentosTransparentes;
    int numSegmentosTransparentes = 0;
    SegmentosTransparentes.push_back(route->getNode(0));

    for ( s = 0; s <= route->getNhops(); s++ ) {
        NoS = route->getNode(s);
        for ( x = s + 1; x <= route->getNhops(); x++ ) {
            NoX = route->getNode(x); //descobre o x-esimo no da rota
            if ( (MAux::Rede.at(NoX).get_NumRegeneradoresDisponiveis() >= RegeneneradoresNecessarios) || (x == route->getNhops()) ) {
                Route rotaQuebrada = *route->breakRoute(NoS, NoX);
                if ( HaEspectroEQualidade(&rotaQuebrada, BitRate, Config, EscolhaEsquemas[m]) ) { //Há qualidade
                    if ( x == route->getNhops() ) { //destino
                        SegmentosTransparentes.push_back(NoX);

                        //Insere conexão na rede
                        long double Tempo = General::exponential(MAux::mu);
                        evt = InserirConexao(route, 0, 1, Tempo, Aux, Config);

                        int hop = 0;
                        for ( int i = 0; i <= numSegmentosTransparentes; i++ ) {
                            SI = -1;
                            Route Segmento = *route->breakRoute(SegmentosTransparentes.at(i), SegmentosTransparentes.at(i + 1));
                            NslotsReq = SlotsReq(BitRate, MelhorEsquema(&Segmento, BitRate, Config));
                            TryToConnect(&Segmento, NslotsReq, NslotsUsed, SI, Config);

                            assert(SI != -1);
                            assert(NslotsReq == NslotsUsed);

                            for ( int c = 0; c < Segmento.getNhops(); c++ ) {
                                evt->conexao->setFirstSlot(hop, SI);
                                evt->conexao->setLastSlot(hop, SI + NslotsReq - 1);
                                L_or = Segmento.getNode(c);
                                L_de = Segmento.getNode(c + 1);
                                for ( int s = evt->conexao->getFirstSlot(hop); s <= evt->conexao->getLastSlot(hop); s++ ) {
                                    assert(Config->Topology_S[s * Def::Nnodes * Def::Nnodes + L_or * Def::Nnodes + L_de] == false);
                                    Config->Topology_S[s * Def::Nnodes * Def::Nnodes + L_or * Def::Nnodes + L_de] = true;
                                    //Os slots sao marcados como ocupados
                                }
                                hop++;
                            }

                            if ( i != 0 ) {
                                int NumReg = MAux::Rede.at(SegmentosTransparentes.at(i)).solicitar_regeneradores(BitRate);
                                assert(NumReg != 0); //Conseguiu solicitar os regeneradores
                                evt->RegeneradoresUtilizados[ SegmentosTransparentes.at(i) ] = NumReg;
                                evt->TotalRegeneradoresUtilizados += NumReg;
                            }
                        }
                        return true;
                    } else {
                        if ( m != 0 ) {
                            r = x;
                            s = x;
                            NoS = route->getNode(s);
                            m = 0;
                            SegmentosTransparentes.push_back(route->getNode(r));
                            numSegmentosTransparentes += 1;
                        } else {
                            r = x; //atualiza ponto de regeneração
                        }
                    }
                } else {
                    if ( r != s ) {
                        numSegmentosTransparentes += 1;
                        SegmentosTransparentes.push_back(route->getNode(r));
                        s = r; //atualiza fonte
                        NoS = route->getNode(s);
                        x = r; //atualiza ponto de teste
                        NoX = route->getNode(x);
                    } else {
                        x -= 1;
                        NoX = route->getNode(x);
                        m += 1;
                        if ( m == numEsquemasDeModulacao )
                            return false; //chamada bloqueada.
                    }
                }
            }
        }
    }
    return false;
}

bool Regeneradores::HaEspectroEQualidade(Route* route, long double BitRate, Def *Config, EsquemaDeModulacao Esquema) {
    int NslotsUsed = 0;
    int NslotsReq;
    int SI = -1; //SlotInicial

    long double OSNR;

    NslotsReq = SlotsReq(BitRate, Esquema);
    TryToConnect(route, NslotsReq, NslotsUsed, SI, Config);
    OSNR = AvaliarOSNR(route, Config);

    if ( (NslotsUsed != 0) && (OSNR >= Def::getlimiarOSNR(Esquema, BitRate)) ) return true;
    return false;
}

bool Regeneradores::HaEspectroEQualidade(Route* route, long double BitRate, Def * Config) {
    EsquemaDeModulacao Esquemas[numEsquemasDeModulacao] = {_64QAM, _16QAM, _4QAM};

    for ( int i = 0; i < numEsquemasDeModulacao; i++ ) {
        if ( Regeneradores::HaEspectroEQualidade(route, BitRate, Config, Esquemas[i]) ) return true;
    }

    return false;
}

EsquemaDeModulacao Regeneradores::MelhorEsquema(Route* route, long double BitRate, Def * Config) {
    EsquemaDeModulacao Esquemas[numEsquemasDeModulacao] = {_64QAM, _16QAM, _4QAM};
    int i;

    for ( i = 0; i < numEsquemasDeModulacao; i++ ) {
        if ( Regeneradores::HaEspectroEQualidade(route, BitRate, Config, Esquemas[i]) ) {
            return Esquemas[i];
        }
    }

    assert(i != numEsquemasDeModulacao - 1);
    return (EsquemaDeModulacao) - 1;
}
