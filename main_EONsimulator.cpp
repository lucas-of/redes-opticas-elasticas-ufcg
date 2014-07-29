//Includes
#include <assert.h>
#include <climits>
#include <fstream>
#include <iostream>
#include <limits>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "Conexao.h"
#include "Def.h"
#include "General.h"
#include "Heuristics.h"
#include "Main_Auxiliar.h"
#include "Route.h"

using namespace std;

//Protótipos de Funções
void AccountForBlocking(int NslotsReq, int NslotsUsed); //Realiza ações necessárias para quando uma conexão foi bloqueada
bool checkFitSi(const bool* vetDisp, int s, int NslotsReq); //Indica se a conexao pode ser inserida em [s:s+NslotsReq]
bool CheckSlotAvailability(const Route*, const int s); //Checa se a rota route está disponível para o uso, com o slot s livre em toda a rota
void clearMemory(); //Limpa e zera todas as constantes de Def.h, reinicia o tempo de simulação e libera todos os slots.
void CompressCon(Event*); /*Diminui a quantidade de slots reservados para a conexão. Está desabilitado atualmente! (assert(false)). Pode ser configurada para retirar sempre o slot da direita ou o da esquerda ou o da direita, escolhido aleatoriamente. Lembrar de conferir isto.*/
void CompressRandom(Conexao *con); /*Comprime conexão, removendo slot da esquerda ou da direita*/
void CompressRight(Conexao *con); /*Comprime conexão, removendo o slot da direita*/
void createStructures();
void DefineNextEventOfCon(Event* evt);
void ExpandCon(Event*);
bool FillSlot(const Route* route, const int s, const bool b);
void loadTopology();
void Load();
void ReleaseCon(Conexao *);
bool ReleaseSlot(const Route*, int);
void RemoveCon(Event*);
void RequestCon(Event*);
void ScheduleEvent(Event*);
void setReqEvent(Event*, TIME);
void Sim();
void SimCompFFO();
void Simulate();
int SlotsReq();
int sumOccupation(int s);
void TryToConnect(const Route* route, const int NslotsReq, int& NslotsUsed, int& si);

void Dijkstra();
void DijkstraAcum(const int orN, const int deN, const int L);
void DijkstraFormas(const int orN, const int deN, const int L);
void FirstFit(const Route*, const int NslotsReq, int& NslotsUsed, int& si);
void FirstFitOpt(const Route*, const int NslotsReq, int& NslotsUsed, int& si);
void MostUsed(const Route*, const int NslotsReq, int& NslotsUsed, int& si);
void MSCL(const Route*, const int NslotsReq, int& NslotsUsed, int& si);
void Random(const Route*, const int NslotsReq, int& NslotsUsed, int& si);
void SDPairReq(int &orN, int &deN);

int main() {
    Load();
    cout << "Inicio da simulacao:" << endl;
    createStructures();
    Dijkstra();
    //Simulacao para varios trafegos
    for(laNet = LaNetMin; laNet <= LaNetMax; laNet += LaPasso) {
        Sim();
        //SimCompFFO(); Simula usando as listas FF otimizadas
    }

    delete []Topology;
    delete []Topology_S;
    delete []AllRoutes;
    if (FFlists != NULL) delete FFlists;
    cout << "Fim do programa";
    cin.get();
    cin.get();
    return 0;
}

void AccountForBlocking(int NslotsReq, int NslotsUsed) {
    if(NslotsUsed <= 0) //A conexao foi bloqueada
        Def::numReq_Bloq++;
    Def::numSlots_Bloq += (NslotsReq - NslotsUsed);
}

bool checkFitSi(const bool* vetDisp, int s, int NslotsReq) {
    assert(s + NslotsReq - 1 < Def::getSE());
    for(int b = s; b < s + NslotsReq; b++)
        if(vetDisp[b] == false)
            return false;
    return true;
}

bool CheckSlotAvailability(const Route* route, const int s) {
    assert(s >=0 && s < Def::getSE());
    int L_or, L_de;
    for(unsigned int c = 0; c < route->getNhops(); c++) {
        L_or = route->getNode(c);
        L_de = route->getNode(c+1);
        if(Topology_S[s*Def::getNnodes()*Def::getNnodes() + L_or*Def::getNnodes() + L_de] == true)
            return false; //Basta o slot s nao estar disponivel em uma das fibras da rota;
    }
    return true; //O slot s esta disponivel em todas as fibras da rota;
}

void clearMemory() {
    int s, L_or, L_de;
    Conexao *con;
    Event *evtPtr;
    const Route* route;
    Def::numReq = Def::numReq_Bloq = Def::numSlots_Req = Def::numSlots_Bloq = Def::numHopsPerRoute =     Def::netOccupancy = simTime = 0.0;
    while(firstEvent != NULL) {
        if(firstEvent->conexao != NULL) {
            //Há uma conexao
            con = firstEvent->conexao;
            route = con->getRoute();
            for (unsigned c = 0; c < route->getNhops(); c++) {
                //remove todos os slots ocupados da conexao
                L_or = route->getNode(c);
                L_de = route->getNode(c+1);
                for (s = con->getFirstSlot(); s <= con->getLastSlot(); s++)
                    Topology_S[s*Def::getNnodes()*Def::getNnodes() + L_or*Def::getNnodes() + L_de] = false;
            }
            delete con;
        }
        evtPtr = firstEvent;
        firstEvent = firstEvent->nextEvent;
        delete evtPtr;
    }

    //Checar se limpeza foi realizada corretamente
    for(int orN = 0; orN < Def::getNnodes(); orN++)
        for(int deN = 0; deN < Def::getNnodes(); deN++)
            for(s = 0; s < Def::getSE(); s++)
                assert(Topology_S[s*Def::getNnodes()*Def::getNnodes() + orN*Def::getNnodes() + deN] == false);
    assert(firstEvent == NULL);
}

void CompressCon(Event* evt) {
    assert(false); //Tirar isso aqui quando quiser considerar Expansao e Compressao

    //Remover um slot lateral
    Conexao *con = evt->conexao;
    CompressRight(con); //Remove sempre o slot da direita. Tentar outras heuristicas
    //CompressRandom(con);
    assert(con->getLastSlot() >= con->getFirstSlot()); //Apenas para checar erros
    DefineNextEventOfCon(evt);
    ScheduleEvent(evt);
}

void CompressRandom(Conexao *con)  {
    //Remove aleatoriamente o slot da direita ou da esquerda.
    if(rand() % 2) {
        //Comprime a esquerda
        FillSlot(con->getRoute(), con->getFirstSlot(), false);
        con->incFirstSlot(+1);
    } else {
        //Comprime a direita
        FillSlot(con->getRoute(), con->getLastSlot(), false);
        con->incLastSlot(-1);
    }
}

void CompressRight(Conexao *con) {
    //Remove sempre o slot da direita.
    FillSlot(con->getRoute(), con->getLastSlot(), false);
    con->incLastSlot(-1);
}

void Load()
{
    int Npontos;
    int aux;
    //Adquire o numero de Nos:
    Topol>>aux;
    Def::setNnodes(aux);
    cout<<"Numero de nos: "<<Def::getNnodes()<<endl;
    Topol>>aux;
    Def::setSE(aux);
    cout<<"Numero de Slots por Enlace: "<<Def::getSE()<<endl;
    //Other inputs to the simulator:
    cout<<"Entre com a Quantidade maxima de Slots por Requisicao [1 <= SR <= "<<Def::getSE()<<"]: ";
    cin>> aux;
    Def::setSR(aux); //Uma requisicao nao podera pedir mais que aux slots
    cout<<DJK<<" - DJK \n"<<DJK_Formas<<" - DJK_Formas \n"<< DJK_Acum<<" - DijkstraAcumulado "<<endl;
    cout<<"Entre com o Alg_Routing:";
    cin >> Alg_Routing;
    cout<<RD<<" - Random \n"<<FF<<" - FirstFit \n"<<MU<<" - Most Used \n"<<FFO<<" - FirstFitOpt "<<endl;
    cout<<"Entre com o AlgAloc: ";
    cin>> Alg_Aloc;
    if(Alg_Aloc == FFO || Alg_Aloc == FF)
    {
        FFlists = new vector<int>*[Def::getSR()+1];
        for(int i = 0; i < Def::getSR()+1; i++)
            FFlists[i] = new vector<int>(0);
    }
    cout<<"Entre com o mu: ";
    cin>> mu; //mu = taxa de desativacao das conexoes;
    cout<<"LaNet Min= ";
    cin >> LaNetMin; // La = taxa de chegada das conexoes;
    cout<<"LaNet Max= ";
    cin >> LaNetMax;
    cout<<"#Pontos no grafico= ";
    cin >> Npontos;
    LaPasso = (LaNetMax-LaNetMin)/(Npontos-1);
    //Dados para a expansao e compressao das conexoes:
    cout<<"Sera considerado Expansao e Compressao do numero de subportadoras das Conexoes? <0>Falso; <1>Verdadeiro;"<<endl;
    cin>> ExpComp;
    if(ExpComp)
    {
        cout<<"Entre com o muC: ";
        cin>> muC;
        cout<<"Entre com o LaE: ";
        cin >> laE;
    }
    long double nR;
    cout<<"Entre com a #Req.: ";
    cin>> nR;
    Def::setNumReqMax(nR);
    //
    for(int f = 0; f <= Def::getSE()-Def::getSR()+1; f++)
        Metrica<<f<<"\t"<<1.0/(f+1)<<endl;

}

// ------------------------------------------------------------- //
void Simulate()
{
    clearMemory();
    //Cria o primeiro evento da rede como uma requisicao:
    firstEvent = new Event;
    setReqEvent(firstEvent, simTime);
    while(Def::numReq < Def::getNumReqMax())
    {
        Event *curEvent = firstEvent;
        firstEvent = firstEvent->nextEvent;
        simTime = curEvent->time;
        if(curEvent->type == Req)
        {
            RequestCon(curEvent);
            //cout<<"numReq= "<<numReq<<endl;
        }
        else if(curEvent->type == Desc)
        {
            RemoveCon(curEvent);
        }
        else if(curEvent->type == Exp)
        {
            assert(ExpComp); //Um evento deste tipo so pode ocorrer se ExpComp=true;
            ExpandCon(curEvent);
        }
        else if(curEvent->type == Comp)
        {
            assert(ExpComp); //Um evento deste tipo so pode ocorrer se ExpComp=true;
            CompressCon(curEvent);
        }
    }
    cout <<"Simulation Time= " << simTime << "  numReq=" << Def::numReq << endl;
    cout << "nu0= " << laNet << "   PbReq= " << (long double) Def::numReq_Bloq/Def::numReq <<
         "   PbSlots= " << (long double) Def::numSlots_Bloq/Def::numSlots_Req << " HopsMed= " <<
         (long double) Def::numHopsPerRoute/(Def::numReq-Def::numReq_Bloq) << " netOcc= " <<
         (long double) Def::netOccupancy << endl;
    Resul << laNet << "\t" << (long double) Def::numReq_Bloq/Def::numReq << "\t" <<
          (long double) Def::numSlots_Bloq/Def::numSlots_Req << "\t" <<
          (long double) Def::numHopsPerRoute/(Def::numReq-Def::numReq_Bloq) << "\t" << Def::netOccupancy << endl;
}

//////////////////////////////////////////////////////////////////////////////////
void Sim()
{
    //Indica como o trafego e distribuido entre s=1,2,...,SE
    Def::setLaUniform(laNet);
    //Def::setLaRandom(laNet);
    if(Alg_Aloc == FFO)
    {
        //Indica uma Heuristica:
        Heuristics::FFO_convencional(FFlists);
        //Heuristics::FFO_extremos(FFlists);
        //Heuristics::FFO_invertido(FFlists);
        //Heuristics::FFO_metrica(FFlists);
    }
    //
    Simulate();
}

// ---------------------------------------------------------------------------- //
void SimCompFFO()
{
    long double Pb_conv, Pb_ext, Pb_met;
    long double difPerc, difPerc_FFOmet_FFOext_Pos = 0.0, difPerc_FFOmet_FFOext_Neg = 0.0,	difPerc_FFOconv_FFOext_Pos = 0.0, difPerc_FFOconv_FFOext_Neg = 0.0;
    for(int it = 0; it < 100; it++)
    {
        Def::setLaRandom(laNet);
        //Testa as heuristicas
        Heuristics::FFO_convencional(FFlists);
        Simulate();
        Pb_conv = Def::numReq_Bloq/Def::numReq;
        //
        Heuristics::FFO_extremos(FFlists);
        Simulate();
        Pb_ext = (double)Def::numReq_Bloq/Def::numReq;
        //
        Heuristics::FFO_metrica(FFlists);
        Simulate();
        Pb_met = Def::numReq_Bloq/Def::numReq;
        //Calcular as diferencas percentuais:
        //Entre FFOmetrica e FFOextremos
        if( (Pb_met > 1000*(1.0/Def::numReq)) && (Pb_ext > 1000*(1.0/Def::numReq)) )  //Tenho confianca nos resultados
        {
            difPerc = (Pb_met - Pb_ext)/Pb_ext;
            if(difPerc > difPerc_FFOmet_FFOext_Pos)
            {
                difPerc_FFOmet_FFOext_Pos = difPerc;
                for(int Lr = 1; Lr <= Def::getSR(); Lr++)
                    ResulFFOmet_FFOext<<Def::getLaNet(Lr)<<" ";
                ResulFFOmet_FFOext<<endl;
            }
            if(difPerc < difPerc_FFOmet_FFOext_Neg)
            {
                difPerc_FFOmet_FFOext_Neg = difPerc;
                for(int Lr = 1; Lr <= Def::getSR(); Lr++)
                    ResulFFOext_FFOmet<<Def::getLaNet(Lr)<<" ";
                ResulFFOext_FFOmet<<endl;
            }
            //
            difPerc = (Pb_conv - Pb_ext)/Pb_ext;
            if(difPerc > difPerc_FFOconv_FFOext_Pos)
            {
                difPerc_FFOconv_FFOext_Pos = difPerc;
                for(int Lr = 1; Lr <= Def::getSR(); Lr++)
                    ResulFFOconv_FFOext<<Def::getLaNet(Lr)<<" ";
                ResulFFOconv_FFOext<<endl;
            }
            if(difPerc < difPerc_FFOconv_FFOext_Neg)
            {
                difPerc_FFOconv_FFOext_Neg = difPerc;
                for(int Lr = 1; Lr <= Def::getSR(); Lr++)
                    ResulFFOext_FFOconv<<Def::getLaNet(Lr)<<" ";
                ResulFFOext_FFOconv<<endl;
            }
        }
    }
    cout<<"Maxima diferenca percentual Positiva entre FFOmet e FFOext: "<<difPerc_FFOmet_FFOext_Pos<<endl;
    cout<<"Maxima diferenca percentual Negativa entre FFOmet e FFOext: "<<difPerc_FFOmet_FFOext_Neg<<endl;
    cout<<"Maxima diferenca percentual Positiva entre FFOext e FFOconv: "<<difPerc_FFOconv_FFOext_Pos<<endl;
    cout<<"Maxima diferenca percentual Negativa entre FFOext e FFOconv: "<<difPerc_FFOconv_FFOext_Neg<<endl;
    Resul2<<"Maxima diferenca percentual Positiva entre FFOmet e FFOext: "<<difPerc_FFOmet_FFOext_Pos<<endl;
    Resul2<<"Maxima diferenca percentual Negativa entre FFOmet e FFOext: "<<difPerc_FFOmet_FFOext_Neg<<endl;
    Resul2<<"Maxima diferenca percentual Positiva entre FFOext e FFOconv: "<<difPerc_FFOconv_FFOext_Pos<<endl;
    Resul2<<"Maxima diferenca percentual Negativa entre FFOext e FFOconv: "<<difPerc_FFOconv_FFOext_Neg<<endl;
}

//////////////////////////////////////////////////////////////////////////////////
void createStructures()
{
    Topology = new long double[Def::getNnodes()*Def::getNnodes()];
    Topology_S = new bool[Def::getNnodes()*Def::getNnodes()*Def::getSE()];
    //Load Topology from input file:
    AllRoutes = new vector<Route*>[Def::getNnodes()*Def::getNnodes()];
    int orN, deN;
    for(orN = 0; orN < Def::getNnodes(); orN++)
    {
        for(deN = 0; deN < Def::getNnodes(); deN++)
        {
            Topol>>Topology[orN*Def::getNnodes()+deN];
            cout<<Topology[orN*Def::getNnodes()+deN]<<"  ";
        }
        cout<<endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////////
void setReqEvent(Event* evt, TIME t)
{
    evt->time = t;
    evt->type = Req;
    evt->nextEvent = NULL;
    evt->conexao = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////
void RequestCon(Event* evt)
{
    int orN, deN, NslotsReq, NslotsUsed, si;
    SDPairReq(orN, deN);
    deN = (orN + Def::getNnodes()/2)%Def::getNnodes(); //Nos antipodas no anel
    //orN = 0;
    //deN = 3;
    NslotsReq = SlotsReq();
    //NslotsReq = 1;
    //cout<<"orN= "<<orN<<"  deN= "<<deN<<"  NslotsReq= "<<NslotsReq<<endl;
    //cin.get();
    Def::numReq++;
    Def::numSlots_Req += NslotsReq;
    if(Alg_Routing == DJK_Formas)
        DijkstraFormas(orN, deN, NslotsReq);
    if(Alg_Routing == DJK_Acum)
        DijkstraAcum(orN, deN, NslotsReq);

    //Para o conjunto de rotas fornecida pelo roteamento, tenta alocar a requisicao:
    Route *route;
    bool success = false;
    for(unsigned int i = 0; i < AllRoutes[orN*Def::getNnodes()+deN].size(); i++)
    {
        route = AllRoutes[orN*Def::getNnodes()+deN].at(i); //Tenta a i-esima rota destinada para o par orN-deN
        //route->print(); cout<<endl;
        NslotsUsed = 0;
        si = -1;
        TryToConnect(route, NslotsReq, NslotsUsed, si);
        assert( (NslotsUsed == 0) || (NslotsUsed == NslotsReq) ); //Tirar isso aqui quando uma conexao puder ser atendida com um numero menor de slots que o requisitado
        if(NslotsUsed > 0)  //A conexao foi aceita
        {
            assert(NslotsUsed <= NslotsReq && si >= 0 && si <= Def::getSE()-NslotsUsed);
            //Inserir a conexao na rede
            int L_or, L_de;
            for(unsigned c = 0; c < route->getNhops(); c++)
            {
                L_or = route->getNode(c);
                L_de = route->getNode(c+1);
                for(int s = si; s < si + NslotsUsed; s++)
                {
                    assert(Topology_S[s*Def::getNnodes()*Def::getNnodes() + L_or*Def::getNnodes() + L_de] == false);
                    Topology_S[s*Def::getNnodes()*Def::getNnodes() + L_or*Def::getNnodes() + L_de] = true; //Os slots s�o ocupados
                }
            }
            Def::numHopsPerRoute += route->getNhops();
            Def::netOccupancy += NslotsUsed*route->getNhops();
            //Cria uma nova conexao
            Conexao *newConexao = new Conexao(route, si, si + NslotsUsed - 1, simTime + General::exponential(mu));
            //Agendar um dos eventos possiveis para conexao (Expandir, contrair, cair, etc):
            Event *evt = new Event;
            evt->conexao = newConexao;
            DefineNextEventOfCon(evt);
            ScheduleEvent(evt);
            break;
        }
    }
    //Verifica quantas conexoes e quantos slots foram bloqueados
    AccountForBlocking(NslotsReq, NslotsUsed);
    //Define o novo evento de chegada de requisicao
    long double IAT = General::exponential(laNet); //Inter-arrival time
    setReqEvent(evt, simTime + IAT);
    assert(evt->type == Req);
    ScheduleEvent(evt); //Reusa este mesmo objeto evt
}

// -------------------------------------------------------------------------- //
int SlotsReq()
{
    double sum=0.0, x;
    int Lr;
    for(Lr = 1; Lr <= Def::getSR(); Lr++)
        sum += Def::getLaNet(Lr);
    x = General::uniforme(0.0,sum);
    sum = 0.0;
    for(Lr = 1; Lr <= Def::getSR(); Lr++)
    {
        sum += Def::getLaNet(Lr);
        if(x < sum)
            break;
    }
    assert(Lr > 0 && Lr <= Def::getSR());
    return Lr;
    //return 1 + (rand()%Def::getSR());
}

// -------------------------------------------------------------------------- //
void RemoveCon(Event* evt)
{
    ReleaseCon(evt->conexao);
    delete evt;
}

// -------------------------------------------------------------------------- //
void ReleaseCon(Conexao *con) //Retirar a conexao da rede:
{
    for(int s = con->getFirstSlot(); s <= con->getLastSlot(); s++)
    {
        assert( (s >= 0) && (s < Def::getSE()) );
        ReleaseSlot(con->getRoute(), s);
    }
    delete con;
}

// -------------------------------------------------------------------------- //
bool ReleaseSlot(const Route* route, int s)
{
    int L_or, L_de;
    for(unsigned c = 0; c < route->getNhops(); c++)
    {
        L_or = route->getNode(c);
        L_de = route->getNode(c+1);
        assert(Topology_S[s*Def::getNnodes()*Def::getNnodes() + L_or*Def::getNnodes() + L_de] == true); //Just to check errors
        Topology_S[s*Def::getNnodes()*Def::getNnodes() + L_or*Def::getNnodes() + L_de] = false;
    }
    return true;
}

// -------------------------------------------------------------------------- //
void ExpandCon(Event* evt)
{
    assert(false); //Tirar isso aqui quando quiser considerar Expansao e Compressao
    Conexao *con = evt->conexao;
    //Procura um slot lateral. Assume-se que tenta primeiro o slot a esquerda e depois o a direita.
    if(con->getFirstSlot() > 0 && CheckSlotAvailability(con->getRoute(), con->getFirstSlot()-1))
    {
        FillSlot(con->getRoute(), con->getFirstSlot()-1, true);
        con->incFirstSlot(-1);
    }
    else if(con->getLastSlot() < Def::getSE()-1 && CheckSlotAvailability(con->getRoute(), con->getLastSlot()+1))
    {
        FillSlot(con->getRoute(), con->getLastSlot()+1, true);
        con->incLastSlot(+1);
    }
    else
        Def::numSlots_Bloq++;
    Def::numSlots_Req++;
    DefineNextEventOfCon(evt);
    ScheduleEvent(evt);
}

// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
bool FillSlot(const Route* route, const int s, const bool b)
{
    int L_or, L_de;
    for(unsigned c = 0; c < route->getNhops(); c++)
    {
        L_or = route->getNode(c);
        L_de = route->getNode(c+1);
        assert(Topology_S[s*Def::getNnodes()*Def::getNnodes() + L_or*Def::getNnodes() + L_de] != b);
        Topology_S[s*Def::getNnodes()*Def::getNnodes() + L_or*Def::getNnodes() + L_de] = b;
    }
    return true;
}

// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //


///////////////////////////////////////////////////////////////////////////////////////
void DefineNextEventOfCon(Event* evt)
{
    Conexao *con = evt->conexao;
    //Recalcula Te (tempo de expansao) e Tc (tempo de compressao) e os compara com o tempo para desativacao
    //So pode haver expansao se o numero de slots for menor do que Def::getSE()
    TIME Te, Tc, evtTime;
    EventType evtType = UNKNOWN;
    if( (ExpComp == true) && ((con->getLastSlot() - con->getFirstSlot() + 1) < Def::getSE()) ) //Se o numero de slots utilizados for menor do que Def::getSE()
        Te = simTime + General::exponential(laE);
    else
        Te = Def::MAX_DOUBLE;
    //So pode haver compressao se o numero de slots for maior ou igual a 2, pois 1 e o numero minimo do slots que podem estar ativos para uma conexao
    if( (ExpComp == true) && (con->getLastSlot() - con->getFirstSlot() + 1 > 1) )
        Tc = simTime + General::exponential(muC);
    else
        Tc = Def::MAX_DOUBLE;
    //Checa o proximo evento dentre expansao, compressao e desativacao
    if(Te < Tc)
    {
        if(Te < con->getTimeDesc())
        {
            evtTime = Te;
            evtType = Exp;
        }
        else
        {
            evtTime = con->getTimeDesc();
            evtType = Desc;
        }
    }
    else
    {
        if(Tc < con->getTimeDesc())
        {
            evtTime = Tc;
            evtType = Comp;
        }
        else
        {
            evtTime = con->getTimeDesc();
            evtType = Desc;
        }
    }
    //
    evt->time = evtTime;
    evt->type = evtType;
}

// -------------------------------------------------------------------------- //
void ScheduleEvent(Event *evt)
{
    Event *evtAux = firstEvent, *evtAnt = NULL;
    while(evtAux != NULL)
    {
        if(evt->time < evtAux->time)
            break;
        else
        {
            evtAnt = evtAux;
            evtAux = evtAux->nextEvent;
        }
    }
    evt->nextEvent = evtAux;
    if(evtAnt == NULL)
        firstEvent = evt;
    else
        evtAnt->nextEvent = evt;
}

///////////////////////////////////////////////////////////////////////////////////////


// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
void TryToConnect(const Route* route, const int NslotsReq, int& NslotsUsed, int& si)   //NslotsReq informa a quantidade de slots requisitados para a conexao;
{
    //si informa o slot inicial da sequencia de slots utilizada;
    si = -1, NslotsUsed = 0; //1 <= NslotsUsed <= Nslots informa quantos slots sao utilizados para a conexao
    switch(Alg_Aloc)
    {
    case RD:
        Random(route, NslotsReq, NslotsUsed, si);
        break;
    case FF:
        FirstFit(route, NslotsReq, NslotsUsed, si);
        break;
    case MU:
        MostUsed(route, NslotsReq, NslotsUsed, si);
        break;
    case FFO:
        FirstFitOpt(route, NslotsReq, NslotsUsed, si);
        break;
    default:
        cout<<"Algoritmo nao definido"<<endl;
    }
} // Fim da funcao Conectar

// -------------------------------------------------------------------------- //


// -------------------------------------------------------------------------- //
void Dijkstra()
{
    int orN, deN, VA, i, j, k, path, h, hops;
    long double min;
    vector<int> r;
    long double *CustoVertice = new long double[Def::getNnodes()];
    int *Precedente = new int[Def::getNnodes()];
    int *PathRev = new int[Def::getNnodes()];
    bool *Status = new bool[Def::getNnodes()];
    //Busca para todos os pares de no a rota mais curta:
    for(orN = 0; orN < Def::getNnodes(); orN++)
    {
        for(i = 0; i < Def::getNnodes(); i++)
        {
            if(i != orN)
                CustoVertice[i] = Def::MAX_DOUBLE;
            else
                CustoVertice[i] = 0.0;
            Precedente[i] = -1;
            Status[i] = 0;
        }
        VA = Def::getNnodes();
        while(VA > 0)
        {
            min = Def::MAX_DOUBLE;
            for(i = 0; i < Def::getNnodes(); i++)
                if((Status[i] == 0)&&(CustoVertice[i] < min))
                {
                    min = CustoVertice[i];
                    k = i;
                }
            Status[k] = 1;
            VA = VA-1;
            for(j = 0; j < Def::getNnodes(); j++)
                if((Status[j] == 0)&&(Topology[k*Def::getNnodes()+j] != 0)&&(CustoVertice[k]+Topology[k*Def::getNnodes()+j] < CustoVertice[j]))
                {
                    CustoVertice[j] = CustoVertice[k]+Topology[k*Def::getNnodes()+j];
                    Precedente[j] = k;
                }
        }//Fim do while
        for(deN = 0; deN < Def::getNnodes(); deN++)
        {
            path = orN*Def::getNnodes()+deN;
            AllRoutes[path].clear();
            if(deN != orN)
            {
                PathRev[0] = deN;
                hops = 0;
                j = deN;
                while(j != orN)
                {
                    hops = hops+1;
                    PathRev[hops] = Precedente[j];
                    j = Precedente[j];
                }
                r.clear();
                for(h = 0; h <= hops; h++)
                    r.push_back(PathRev[hops-h]);
                AllRoutes[path].push_back(new Route(r));
            }
        }
    }
    //Imprimir Rotas:
    for(orN = 0; orN < Def::getNnodes(); orN++)
        for(deN = 0; deN < Def::getNnodes(); deN++)
            if(orN != deN)
            {
                cout<<endl<<"[orN="<<orN<<"  deN="<<deN<<"]  route= ";
                path = orN*Def::getNnodes()+deN;
                hops = AllRoutes[path].at(0)->getNhops();
                cout<<hops<<":";
                if(hops != 0)
                    for(h = 0; h <= hops; h++)
                        cout<<AllRoutes[path].at(0)->getNode(h)<<"-";
            }
    cout<<endl<<endl;
    delete []CustoVertice;
    delete []Precedente;
    delete []Status;
    delete []PathRev;
}

// -------------------------------------------------------------------------- //
void DijkstraFormas(const int orN, const int deN, const int L) 	//L e a largura de banda (em numero de slots) da requisicao
{
    assert(orN != deN);
    int VA, i, j, k, path, h, hops;
    long double min;
    bool *DispLink = new bool[Def::getSE()];
    long double *CustoVertice = new long double[Def::getNnodes()];
    long double custoLink;
    int *Precedente = new int[Def::getNnodes()];
    int *PathRev = new int[Def::getNnodes()];
    bool *Status = new bool[Def::getNnodes()];
    //Busca para todos os pares de no a rota mais curta:
    for(i = 0; i < Def::getNnodes(); i++)
    {
        if(i != orN)
            CustoVertice[i] = Def::MAX_DOUBLE;
        else
            CustoVertice[i] = 0.0;
        Precedente[i] = -1;
        Status[i] = 0;
    }
    VA = Def::getNnodes();
    while(VA > 0)
    {
        //Procura o vertice de menor custo
        min = Def::MAX_DOUBLE;
        for(i = 0; i < Def::getNnodes(); i++)
            if((Status[i] == 0)&&(CustoVertice[i] < min))
            {
                min = CustoVertice[i];
                k = i;
            }
        Status[k] = 1; //k e o vertice de menor custo;
        VA = VA-1;
        //Verifica se precisa atualizar ou nao os vizinhos de k
        for(j = 0; j < Def::getNnodes(); j++)
            if((Status[j] == 0)&&(Topology[k*Def::getNnodes()+j] != 0))  //O no j e nao marcado e vizinho do no k
            {
                //Calcula O vetor de disponibilidade do enlace entre k e j
                for(int s = 0; s < Def::getSE(); s++)
                    DispLink[s] = !Topology_S[s*Def::getNnodes()*Def::getNnodes() + k*Def::getNnodes() + j];
                custoLink = Heuristics::calculateCostLink(DispLink, L);
                if(CustoVertice[k] + custoLink < CustoVertice[j])
                {
                    CustoVertice[j] = CustoVertice[k] + custoLink;
                    Precedente[j] = k;
                }
            }
    }//Fim do while
    //Formar a rota:
    path = orN*Def::getNnodes()+deN;
    AllRoutes[path].clear();
    PathRev[0] = deN;
    hops = 0;
    j = deN;
    while(j != orN)
    {
        hops = hops+1;
        PathRev[hops] = Precedente[j];
        j = Precedente[j];
    }
    vector<int> r;
    r.clear();
    for(h = 0; h <= hops; h++)
        r.push_back(PathRev[hops-h]);
    assert(r.at(0) == orN && r.at(hops) == orN);
    AllRoutes[path].push_back(new Route(r));
    //
    delete []CustoVertice;
    delete []Precedente;
    delete []Status;
    delete []PathRev;
    delete []DispLink;
}

// -------------------------------------------------------------------------- //
void DijkstraAcum(const int orN, const int deN, const int L) //L e a largura de banda (em numero de slots) da requisicao
{
    assert(deN != orN);
    int VA, i, j, s, k, h, path, hops, hopsAux;
    long double min, custoAux;
    vector<int> r;
    long double *CustoVertice = new long double[Def::getNnodes()];
    bool *DispVertice = new bool[Def::getNnodes()*Def::getSE()];
    int *HopsVertice = new int[Def::getNnodes()]; //O numero de hops ate chegar aquele no pelo caminho mais curto
    bool *DispAux = new bool[Def::getSE()];
    int *Precedente = new int[Def::getNnodes()]; //Informa em cada no por onde chegou a rota mais curta
    int *PathRev = new int[Def::getNnodes()];
    bool *Status = new bool[Def::getNnodes()]; //Informa os nos marcados e nao marcados
    //Busca a rota mais curta para os nos orN e deN;
    //Inicializacao do algoritmo:
    for(i = 0; i < Def::getNnodes(); i++)
    {
        if(i != orN)
        {
            CustoVertice[i] = Def::MAX_DOUBLE;
            HopsVertice[i] = Def::MAX_INT;
            for(s = 0; s < Def::getSE(); s++)
                DispVertice[i*Def::getSE() + s] = false; //O slot e assumido indisponivel
        }
        else
        {
            CustoVertice[i] = 0.0;
            HopsVertice[i] = 0;
            for(s = 0; s < Def::getSE(); s++)
                DispVertice[i*Def::getSE() + s] = true; //O slot e assumido disponivel
        }
        Precedente[i] = -1;
        Status[i] = 0; //No ainda nao foi marcado
    }
    //Processo de Dijkstra:
    VA = Def::getNnodes();
    while(VA > 0)
    {
        //Toma o no de menor custo ainda nao marcado
        min = Def::MAX_DOUBLE;
        for(i = 0; i < Def::getNnodes(); i++)
            if((Status[i] == false)&&(CustoVertice[i] < min))
            {
                min = CustoVertice[i];
                k = i; //No k e o de menor custo
            }
        Status[k] = true; //No k passa a ser marcado
        //A partir do no k, atualiza ou nao o custo de seus nos vizinhos (j)
        VA = VA-1;
        for(j = 0; j < Def::getNnodes(); j++)
            if((Status[j] == false)&&(Topology[k*Def::getNnodes()+j] != 0)) //O no j e nao marcado e vizinho do no k
            {
                //Calcula O vetor de disponibilidade em j por uma rota proveniente de k
                for(s = 0; s < Def::getSE(); s++)
                    DispAux[s] = DispVertice[k*Def::getSE() + s] * !Topology_S[s*Def::getNnodes()*Def::getNnodes() + k*Def::getNnodes() + j];
                //Calcula o numero de hops em j por uma rota proveniente de k
                hopsAux = HopsVertice[k]+1;
                //Checa se o custo em j deve ser atualizado
                custoAux = Heuristics::calculateCostRouting(hopsAux, DispAux, L);
                if(custoAux < CustoVertice[j])
                {
                    HopsVertice[j] = hopsAux;
                    CustoVertice[j] = custoAux;
                    for(int s = 0; s < Def::getSE(); s++)
                        DispVertice[j*Def::getSE() + s] = DispAux[s];
                    Precedente[j] = k;
                }
            }
    }//Fim do while
    //Descrever a rota selecionada:
    PathRev[0] = deN;
    hops = 0;
    j = deN;
    while(j != orN)
    {
        hops = hops+1;
        PathRev[hops] = Precedente[j];
        j = Precedente[j];
    }
    r.clear();
    for(h = 0; h <= hops; h++)
        r.push_back(PathRev[hops-h]);
    delete []CustoVertice;
    delete []Precedente;
    delete []Status;
    delete []PathRev;
    delete []DispVertice;
    delete []DispAux;
    delete []HopsVertice;
    //Insere a rota nova em AllRoutes
    path = orN*Def::getNnodes()+deN;
    AllRoutes[path].clear();
    AllRoutes[path].push_back(new Route(r));
}

////////////////////////////////////////////////////////////////////////////////
void SDPairReq(int &orN, int &deN)
{
    //cout<<"Trafego espacial uniforme"<<endl;
    orN = rand()% Def::getNnodes();
    deN = rand()% (Def::getNnodes()-1);
    if(deN >= orN)
        deN += 1;
    if( (orN < 0) || (orN>= Def::getNnodes()) || (deN<0) || (deN>=Def::getNnodes()) ||(deN == orN))
    {
        cout<<"Erro em SDPair";
        cin.get();
    }
}

////////////////////////////////////////////////////////////////////////////////
// -------------------------------------------------------------------------- //
////////////////////////////////////////////////////////////////////////////////
void Random(const Route* route, const int NslotsReq, int& NslotsUsed, int& si)
{
    int soma=0, somaAlocacao=0, alocarSlot;
    bool *vetDisp = new bool[Def::getSE()];
    int *vetAloc = new int[Def::getSE()];
    //
    for(int s = 0; s < Def::getSE(); s++)
        vetAloc[s] = 0;
    //Checa a disponibilidade no caminho 'path' para cada slot s;
    for(int s = 0; s < Def::getSE(); s++)
        vetDisp[s] = CheckSlotAvailability(route, s);//Checa a disponibilidade no caminho 'path' para o slot s;
    //Carrega vetSlotsUsed com o numero de enlaces ocupados em cada slot;
    bool fit;
    NslotsUsed = 0;
    si = -1;
    for(int s = 0; s <= Def::getSE() - NslotsReq; s++)
    {
        fit = true;
        soma = 0;
        for(int b = s; b < s + NslotsReq; b++)
            if(vetDisp[b] == true)
                soma++;
            else
            {
                fit = false;
                break;
            }
        if(fit == true && soma == NslotsReq)  //A requisicao se encaixa entre os slots s,s+1,...,s+NslotsReq-1
        {
            somaAlocacao++; // verifica quantas posicoes estao disponiveis para alocacao
            vetAloc[s] = 1; //posso alocar a partir desta posicao
        }
    }
    // fazer o sorteio do slot que sera alocada
    if(somaAlocacao > 0) // se existir pelo menos um conjunto slot disponivel com o tamanho da solicitacao
    {
        alocarSlot = rand()% somaAlocacao; // fazer alocacao no conjunto de slot dispovivel "alocarSlot"
        int s = 0;
        while ((s < Def::getSE()) && (alocarSlot >= 0))
        {
            if(vetAloc[s] == 1)
                alocarSlot--;
            s++;
        }
        NslotsUsed = NslotsReq;
        si = s-1; // -1 porque foi incrementado no final do while
    }
    else
    {
        NslotsUsed = 0;
        si = -1;
    }
    delete []vetDisp;
    delete []vetAloc;
}

// -------------------------------------------------------------------------- //
void FirstFit(const Route* route, const int NslotsReq, int& NslotsUsed, int& si)
{
    si = -1;
    NslotsUsed = 0; //Valores nao permitidos
    int sum;
    for(int s = 0; s <= Def::getSE() - NslotsReq; s++)
    {
        sum = 0;
        for(int se = s; se < s + NslotsReq; se++)
        {
            if(CheckSlotAvailability(route, se))
                sum++;
            else
                break;
        }
        if(sum == NslotsReq)
        {
            si = s;
            NslotsUsed = NslotsReq;
            break;
        }
    }
}

// -------------------------------------------------------------------------- //
void MostUsed(const Route* route, const int NslotsReq, int& NslotsUsed, int& si)
{
    int *vetSlotsUsed = new int[Def::getSE()];
    bool *vetDisp = new bool[Def::getSE()];
    //Checa a disponibilidade no caminho 'path' para cada slot s;
    for(int s = 0; s < Def::getSE(); s++)
        vetDisp[s] = CheckSlotAvailability(route, s);//Checa a disponibilidade no caminho 'path' para o slot s;
    //Carrega vetSlotsUsed com o numero de enlaces ocupados em cada slot;
    int soma;
    for(int s = 0; s < Def::getSE(); s++)
    {
        if(vetDisp[s] == true)
            vetSlotsUsed[s] = sumOccupation(s);
        else
            vetSlotsUsed[s] = -1;
    }
    //Obtem o slot mais ocupado dentre os que podem receber a requisicao
    bool fit;
    double maxSoma = -1;//Smax*N*N+1;
    NslotsUsed = 0;
    si = -1;
    for(int s = 0; s <= Def::getSE()-NslotsReq; s++)
    {
        fit = true;
        soma = 0;
        for(int b = s; b < s + NslotsReq; b++)
            if(vetDisp[b] == true)
            {
                assert(vetSlotsUsed[b] >= 0);
                soma += vetSlotsUsed[b];
            }
            else
            {
                fit = false;
                break;
            }
        if(fit == true && soma > maxSoma)  //A requisicao se encaixa entre os slots s,s+1,...,s+NslotsReq-1 e nesta posicao havera a maior soma de enlaces ocupados na rede;
        {
            maxSoma = soma;
            NslotsUsed = NslotsReq;
            si = s;
        }
    }
    delete []vetSlotsUsed;
    delete []vetDisp;
}

/*
// ----------------------------------------------------------------------------- //
void MSCL(const Route* route, const int NslotsReq, int& NslotsUsed, int& si){
    bool *vetDisp = new bool[Def::getSE()];
    int s;
    //Checa a disponibilidade no caminho 'path' para cada slot s:
    for(s = 0; s < Def::getSE(); s++)
        vetDisp[s] = checkDisp(route, s); //Checa a disponibilidade na rota 'route' para o slot s;
    //Obtem quais slots podem comecar a requisicao:
    bool *vetDispFitSi = new bool[Def::getSE()];
    for(s = 0; s <= Def::getSE() - NslotsReq; s++)
        vetDispFitSi[s] = checkFitSi(vetDisp, s, NslotsReq); //Se a conexao pode ser inserida em s, s+1,...,s+NslotsReq-1
    //Calcular a disponibilidade de cada caminho que interfere com path
    int H = Route[path*(N+1)]; //H = numero de hops da requisicao
    int r, path_int, L_or, L_de;
    bool vetDispInt[Def::getSE()];
    vector<bool> *RouteDisp = new vector<bool>[RouteInt[path]->size()];
    for(r = 0; r < RouteInt[path]->size(); r++){
        path_int = RouteInt[path]->at(r); //r_int e o indice da rota que interfere com a rota de indice r
        H = Route[path_int*(N+1)]; //H = numero de hops da conexao
        for(s = 0; s < Def::getSE(); s++){
            RouteDisp[r].push_back(true);
            for(int c = 1; c <= H; c++)	{
                L_or = Route[path_int*(N+1)+c];
                L_de = Route[path_int*(N+1)+c+1];
                if(Topology_S[s*Def::getNnodes()*Def::getNnodes() + L_or*Def::getNnodes() + L_de] == true){ //O slot s esta ocupado no enlace L_or->L_de
                    RouteDisp[r].at(s) = false;
                    break;
                }
            }
        }
    }
    //Calcular a perda de cada possibilidade de alocacao:
    int vetCapInic[Def::getSE()+1], vetCapFin[Def::getSE()+1];
    double perda, perdaMin = MAX_DOUBLE;
    for(s = 0; s <= Def::getSE() - NslotsReq; s++)
        if(vetDispFitSi[s] == true){ //O caminho pode ser iniciado neste slot
            perda = 0.0;
            //Obter as rotas interferentes
            for(r = 0; r < RouteInt[path]->size(); r++){
                path_int = RouteInt[path]->at(r); //r_int e o indice da rota que interfere com a rota de indice r
                //Carrega DispInt com a disponibilidade inicial da rota de �ndice r_int:
                for(int se = 0; se < Def::getSE(); se++)
                    vetDispInt[se] = RouteDisp[r].at(se);
                //Calcula a capacidade de alocacao nesta rota:
                calcVetCap(vetDispInt, vetCapInic);
                //Assume a alocacao da rota de indice r nos slots s,s+1,...,s+NslotsReq-1 => retira os slots s,s+1,...,s+NslotsReq-1 de vetDispInt
                for(int i = s; i < s + NslotsReq; i++)
                    vetDispInt[i] = false;
                calcVetCap(vetDispInt, vetCapFin);
                perda += calcPerda(vetCapInic, vetCapFin);
            }
            if(perda < perdaMin){
                perdaMin = perda;
                si = s;
                NslotsUsed = NslotsReq;
            }
        }
    delete []vetDisp;
    delete []vetDispFitSi;
    delete []RouteDisp;
}
*/

// -------------------------------------------------------------------------- //
void FirstFitOpt(const Route* route, const int NslotsReq, int& NslotsUsed, int& si)
{
    assert( (si == -1) && (NslotsUsed == 0) );
    int s, sum;
    for(int sOrd = 0; sOrd < Def::getSE(); sOrd++)
    {
        s = FFlists[NslotsReq]->at(sOrd);
        if(s <= Def::getSE()-NslotsReq)  //si e capaz de suportar a requisicao;
        {
            sum = 0;
            for(int se = s; se < s + NslotsReq; se++)
                if(CheckSlotAvailability(route, se))
                    sum++;
                else
                    break;
        }
        if(sum == NslotsReq)  //O slot s pode atender a requisicao
        {
            si = s;
            NslotsUsed = NslotsReq;
            break;
        }
    }
}

// ****************************************************************** //
int sumOccupation(int s)
{
    int soma=0;
    for(int origem = 0; origem < Def::getNnodes(); origem++)
        for(int destino = 0; destino < Def::getNnodes(); destino++)
            if( (Topology[origem*Def::getNnodes() + destino] > 0.0) && (Topology_S[s*Def::getNnodes()*Def::getNnodes() + origem*Def::getNnodes() + destino] == true) ) //Se houver enlace entre origem e destino e o slot 's' estiver livre neste enlace
                soma++;
    return soma;
}

////////////////////////////////////////////////////////////////////////////////
// -------------------------------------------------------------------------- //
////////////////////////////////////////////////////////////////////////////////


// -------------------------------------------------------------------------- //



// FIM //
