// teste2.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <limits>
#include <climits>
#include <vector>
using namespace std;
 
#include "General.h"
#include "Def.h"
#include "Route.h"
#include "Conexao.h"
//#include "Path.h"
#include "Heuristics.h"

ofstream Resul("Result.txt");
ofstream Metrica("Metrica.txt");
ofstream Resul2("Result2.txt");
ofstream ResulFFOmet_FFOext("ResultFFOmet_FFOext.txt");
ofstream ResulFFOext_FFOmet("ResultFFOext_FFOmet.txt");
ofstream ResulFFOconv_FFOext("ResultFFOconv_FFOext.txt");
ofstream ResulFFOext_FFOconv("ResultFFOext_FFOconv.txt");

ifstream Topol("Topology.txt");
//Definicoes do roteamento:
#define DJK 0 // Dijkstra convencional
#define DJK_Formas 1 //Dijkstra cujo peso do enlace � dado pelo n�mero de formas
#define DJK_Acum 2 // Dijkstra acumulado
//Definicoes da aloca��o de espectro:
#define RD 0 // Random
#define FF 1 // First-Fit
#define MU 2  // Most Used
#define FFO 3 //First-Fit with an optimized list

///////////////////////////////////
enum EventType {UNKNOWN, Req, Desc, Exp, Comp};

struct Event
{
    TIME time;
    EventType type;
    Event *nextEvent;
    Conexao* conexao;
};

///////////////////////////////////
void createStructures();
void Load();
void Simulate();
void Sim();
void SimCompFFO();
void clearStructures();
void clearMemory();
void loadTopology();

void RequestCon(Event*);
void RemoveCon(Event*);
void ExpandCon(Event*);
void CompressCon(Event*);
void CompressRight(Conexao *con);
void CompressRandom(Conexao *con);
void DefineNextEventOfCon(Event* evt);
void setReqEvent(Event*, TIME);
int SlotsReq();
void ReleaseCon(Conexao *);
bool ReleaseSlot(const Route*, int);
bool CheckSlotAvailability(const Route*, const int s);
bool FillSlot(const Route* route, const int s, const bool b);
void DefineNextEventOfCon(Event* evt);
void ScheduleEvent(Event*);
void TryToConnect(const Route* route, const int NslotsReq, int& NslotsUsed, int& si);
void AccountForBlocking(int NslotsReq, int NslotsUsed);



void Dijkstra();
void DijkstraFormas(const int orN, const int deN, const int L);
void DijkstraAcum(const int orN, const int deN, const int L);
void SDPairReq(int &orN, int &deN);
void Random(const Route*, const int NslotsReq, int& NslotsUsed, int& si);
void FirstFit(const Route*, const int NslotsReq, int& NslotsUsed, int& si);
void MostUsed(const Route*, const int NslotsReq, int& NslotsUsed, int& si);
void MSCL(const Route*, const int NslotsReq, int& NslotsUsed, int& si);
void FirstFitOpt(const Route*, const int NslotsReq, int& NslotsUsed, int& si);

bool checkDisp(const Route *route, int s);
int sumOccupation(int s);
bool checkFitSi(const bool* vetDisp, int s, int NslotsReq);
//void loadRouteInt();

TIME simTime;
bool *Topology_S;
long double *Topology;
vector<int>**RouteInt;
Event *firstEvent;
vector<Route*> *AllRoutes;

bool ExpComp;
int Alg_Routing, Alg_Aloc; // Informa o algortimo de Roteamento e de Aloca��o de espectro;
long double mu, laNet, laE, muC; //laE = Taxa de expans�o; muC = Taxa de compress�o
long double LaNetMin, LaNetMax, LaPasso;

bool *Traf;
vector<int>**FFlists = NULL;

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
int main()
{
    Load();
    //In�cio da simula��o:
    cout<<"Inicio da simulacao:"<<endl;
    createStructures();
    clearStructures();
    Dijkstra();
    //loadRouteInt();
    //////Simula��o para v�rias cargas:
    for(laNet = LaNetMin; laNet <= LaNetMax; laNet += LaPasso)
    {
        Sim();
        //SimCompFFO();
    }
    delete []Topology;
    delete []Topology_S;
    delete []AllRoutes;
    if(FFlists != NULL)
        delete FFlists;
    cout<<"Fim do programa";
    cin.get();
    cin.get();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
void Load()
{
    int Npontos;
    int aux;
    //Adquire o n�mero de N�s:
    Topol>>aux;
    Def::setNnodes(aux);
    cout<<"Numero de nos: "<<Def::getNnodes()<<endl;
    Topol>>aux;
    Def::setSE(aux);
    cout<<"Numero de Slots por Enlace: "<<Def::getSE()<<endl;
    //Other inputs to the simulator:
    cout<<"Entre com a Quantidade maxima de Slots por Requisicao [1 <= SR <= "<<Def::getSE()<<"]: ";
    cin>> aux;
    Def::setSR(aux); //Uma requisi��o n�o poder� pedir mais que aux slots
    cout<<DJK<<" - DJK \n"<<DJK_Formas<<" - DJK_Formas \n"<< DJK_Acum<<" - DijkstraAcumulado "<<endl;
    cout<<"Entre com o Alg_Routing:";
    cin>> Alg_Routing;
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
    //Dados para a expans�o e compress�o das conex�es:
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
    //Cria o primeiro evento da rede como uma requisi��o:
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
            assert(ExpComp); //Um evento deste tipo s� pode ocorrer se ExpComp=true;
            ExpandCon(curEvent);
        }
        else if(curEvent->type == Comp)
        {
            assert(ExpComp); //Um evento deste tipo s� pode ocorrer se ExpComp=true;
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
    //Indica como o tr�fego � distribu�do entre s=1,2,...,SE
    Def::setLaUniform(laNet);
    //Def::setLaRandom(laNet);
    //Def::setLaManual(laNet);
    if(Alg_Aloc == FFO)
    {
        //Indica uma Heur�stica:
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
        //Testa as heur�sticas
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
        //Calcular as diferen�as percentuais:
        //Entre FFOmetrica e FFOextremos
        if( (Pb_met > 1000*(1.0/Def::numReq)) && (Pb_ext > 1000*(1.0/Def::numReq)) )  //Tenho confian�a nos resultados
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
    cout<<"Maxima diferen�a percentual Positiva entre FFOmet e FFOext: "<<difPerc_FFOmet_FFOext_Pos<<endl;
    cout<<"Maxima diferen�a percentual Negativa entre FFOmet e FFOext: "<<difPerc_FFOmet_FFOext_Neg<<endl;
    cout<<"Maxima diferen�a percentual Positiva entre FFOext e FFOconv: "<<difPerc_FFOconv_FFOext_Pos<<endl;
    cout<<"Maxima diferen�a percentual Negativa entre FFOext e FFOconv: "<<difPerc_FFOconv_FFOext_Neg<<endl;
    Resul2<<"Maxima diferen�a percentual Positiva entre FFOmet e FFOext: "<<difPerc_FFOmet_FFOext_Pos<<endl;
    Resul2<<"Maxima diferen�a percentual Negativa entre FFOmet e FFOext: "<<difPerc_FFOmet_FFOext_Neg<<endl;
    Resul2<<"Maxima diferen�a percentual Positiva entre FFOext e FFOconv: "<<difPerc_FFOconv_FFOext_Pos<<endl;
    Resul2<<"Maxima diferen�a percentual Negativa entre FFOext e FFOconv: "<<difPerc_FFOconv_FFOext_Neg<<endl;
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
    deN = (orN + Def::getNnodes()/2)%Def::getNnodes(); //N�s ant�podas no anel
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

    //Para o conjunto de rotas fornecida pelo roteamento, tenta alocar a requisi��o:
    Route *route;
    bool success = false;
    for(unsigned int i = 0; i < AllRoutes[orN*Def::getNnodes()+deN].size(); i++)
    {
        route = AllRoutes[orN*Def::getNnodes()+deN].at(i); //Tenta a i-�sima rota destinada para o par orN-deN
        //route->print(); cout<<endl;
        NslotsUsed = 0;
        si = -1;
        TryToConnect(route, NslotsReq, NslotsUsed, si);
        assert( (NslotsUsed == 0) || (NslotsUsed == NslotsReq) ); //Tirar isso aqui quando uma conex�o puder ser atendida com um n�mero menor de slots que o requisitado
        if(NslotsUsed > 0)  //A conex�o foi aceita
        {
            assert(NslotsUsed <= NslotsReq && si >= 0 && si <= Def::getSE()-NslotsUsed);
            //Inserir a conex�o na rede
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
            //Cria uma nova conex�o
            Conexao *newConexao = new Conexao(route, si, si + NslotsUsed - 1, simTime + General::exponential(mu));
            //Agendar um dos eventos poss�veis para conex�o (Expandir, contrair, cair, etc):
            Event *evt = new Event;
            evt->conexao = newConexao;
            DefineNextEventOfCon(evt);
            ScheduleEvent(evt);
            break;
        }
    }
    //Verifica quantas conex�es e quantos slots foram bloqueados
    AccountForBlocking(NslotsReq, NslotsUsed);
    //Define o novo evento de chegada de requisi��o
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
void ReleaseCon(Conexao *con) //Retirar a conex�o da rede:
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
    assert(false); //Tirar isso aqui quando quiser considerar Expans�o e Compress�o
    Conexao *con = evt->conexao;
    //Procura um slot lateral. Assume-se que tenta primeiro o slot � esquerda e depois o � direita.
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
bool CheckSlotAvailability(const Route* route, const int s)
{
    assert(s >=0 && s < Def::getSE());
    int L_or, L_de;
    for(unsigned int c = 0; c < route->getNhops(); c++)
    {
        L_or = route->getNode(c);
        L_de = route->getNode(c+1);
        if(Topology_S[s*Def::getNnodes()*Def::getNnodes() + L_or*Def::getNnodes() + L_de] == true)
            return false; //Basta o slot s n�o estar dispon�vel em uma das fibras da rota;
    }
    return true; //O slot s est� dispon�vel em todas as fibras da rota;
}

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
void CompressCon(Event* evt)
{
    assert(false); //Tirar isso aqui quando quiser considerar Expans�o e Compress�o
    //Remover um slot lateral
    Conexao *con = evt->conexao;
    CompressRight(con); //Remove sempre o slot da direita. Tentar outras heur�sticas
    //CompressRandom(con);
    assert(con->getLastSlot() >= con->getFirstSlot()); //Apenas para checar erros
    DefineNextEventOfCon(evt);
    ScheduleEvent(evt);
}

// -------------------------------------------------------------------------- //
void CompressRight(Conexao *con)  //Remove sempre o slot da direita.
{
    FillSlot(con->getRoute(), con->getLastSlot(), false);
    con->incLastSlot(-1);
}

// -------------------------------------------------------------------------- //
void CompressRandom(Conexao *con)  //Remove aleatoriamente o slot da direita ou da esquerda.
{
    if(rand()%2 == 0)  // Comprime `a esquerda
    {
        FillSlot(con->getRoute(), con->getFirstSlot(), false);
        con->incFirstSlot(+1);
    }
    else  //Comprime `a direita
    {
        FillSlot(con->getRoute(), con->getLastSlot(), false);
        con->incLastSlot(-1);
    }
}

///////////////////////////////////////////////////////////////////////////////////////
void DefineNextEventOfCon(Event* evt)
{
    Conexao *con = evt->conexao;
    //Recalcula Te (tempo de expans�o) e Tc (tempo de compress�o) e os compara com o tempo para desativa��o
    //S� pode haver expans�o se o n�mero de slots for menor do que Def::getSE()
    TIME Te, Tc, evtTime;
    EventType evtType = UNKNOWN;
    if( (ExpComp == true) && ((con->getLastSlot() - con->getFirstSlot() + 1) < Def::getSE()) ) //Se o n�mero de slots utilizados for menor do que Def::getSE()
        Te = simTime + General::exponential(laE);
    else
        Te = Def::MAX_DOUBLE;
    //S� pode haver compress�o se o n�mero de slots for maior ou igual a 2, pois 1 � o n�moer m�nimo do slots que podem estar ativos para uma conex�o
    if( (ExpComp == true) && (con->getLastSlot() - con->getFirstSlot() + 1 > 1) )
        Tc = simTime + General::exponential(muC);
    else
        Tc = Def::MAX_DOUBLE;
    //Checa o pr�ximo evento dentre expans�o, compress�o e desativa��o
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
void clearStructures()  //Para o in�cio geral.
{
    int orN, deN, s;
    for(orN = 0; orN < Def::getNnodes(); orN++)
        for(deN = 0; deN < Def::getNnodes(); deN++)
            for(s = 0; s < Def::getSE(); s++)
                Topology_S[s*Def::getNnodes()*Def::getNnodes() + orN*Def::getNnodes() + deN] = false;
    firstEvent = NULL;
}

// -------------------------------------------------------------------------- //
void clearMemory()
{
    Event *evtPtr;
    int s, L_or, L_de;
    Conexao *con;
    const Route* route;
    Def::numReq = 0.0;
    Def::numReq_Bloq = 0.0;
    Def::numSlots_Req = 0.0;
    Def::numSlots_Bloq = 0.0;
    Def::numHopsPerRoute = 0.0;
    Def::netOccupancy = 0.0;
    simTime = 0.0;
    while(firstEvent != NULL)
    {
        if(firstEvent->conexao != NULL)  //Isto � uma conex�o
        {
            con = firstEvent->conexao;
            route = con->getRoute();
            //remove todos os slots ocupados da conex�o
            for(unsigned c = 0; c < route->getNhops(); c++)
            {
                L_or = route->getNode(c);
                L_de = route->getNode(c+1);
                for(s = con->getFirstSlot(); s <= con->getLastSlot(); s++)
                    Topology_S[s*Def::getNnodes()*Def::getNnodes() + L_or*Def::getNnodes() + L_de] = false;
            }
            delete con;
        }
        evtPtr = firstEvent;
        firstEvent = firstEvent->nextEvent;
        delete evtPtr;
    }
    //Checar se o simulador est� correto:
    for(int orN = 0; orN < Def::getNnodes(); orN++)
        for(int deN = 0; deN < Def::getNnodes(); deN++)
            for(s = 0; s < Def::getSE(); s++)
                assert(Topology_S[s*Def::getNnodes()*Def::getNnodes() + orN*Def::getNnodes() + deN] == false);
    assert(firstEvent == NULL);
} // Fim da funcao

// -------------------------------------------------------------------------- //
void TryToConnect(const Route* route, const int NslotsReq, int& NslotsUsed, int& si)   //NslotsReq informa a quantidade de slots requisitados para a conex�o;
{
    //si informa o slot inicial da sequ�ncia de slots utilizada;
    si = -1, NslotsUsed = 0; //1 <= NslotsUsed <= Nslots informa quantos slots s�o utilizados para a conex�o
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
void AccountForBlocking(int NslotsReq, int NslotsUsed)
{
    if(NslotsUsed <= 0) //A conex�o foi bloqueada
        Def::numReq_Bloq++;
    Def::numSlots_Bloq += (NslotsReq - NslotsUsed);
}

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
    //Busca para todos os pares de n� a rota mais curta:
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
void DijkstraFormas(const int orN, const int deN, const int L) 	//L � a largura de banda (em n�mero de slots) da requisi��o
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
    //Busca para todos os pares de n� a rota mais curta:
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
        //Procura o v�rtice de menor custo
        min = Def::MAX_DOUBLE;
        for(i = 0; i < Def::getNnodes(); i++)
            if((Status[i] == 0)&&(CustoVertice[i] < min))
            {
                min = CustoVertice[i];
                k = i;
            }
        Status[k] = 1; //k � o v�rtice de menor custo;
        VA = VA-1;
        //Verifica se precisa atualizar ou n�o os vizinhos de k
        for(j = 0; j < Def::getNnodes(); j++)
            if((Status[j] == 0)&&(Topology[k*Def::getNnodes()+j] != 0))  //O n� j � n�o marcado e vizinho do n� k
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
void DijkstraAcum(const int orN, const int deN, const int L) //L � a largura de banda (em n�mero de slots) da requisi��o
{
    assert(deN != orN);
    int VA, i, j, s, k, h, path, hops, hopsAux;
    long double min, custoAux;
    vector<int> r;
    long double *CustoVertice = new long double[Def::getNnodes()];
    bool *DispVertice = new bool[Def::getNnodes()*Def::getSE()];
    int *HopsVertice = new int[Def::getNnodes()]; //O n�mero de hops at� chegar �quele n� pelo caminho mais curto
    bool *DispAux = new bool[Def::getSE()];
    int *Precedente = new int[Def::getNnodes()]; //Informa em cada n� por onde chegou a rota mais curta
    int *PathRev = new int[Def::getNnodes()];
    bool *Status = new bool[Def::getNnodes()]; //Informa os n�s marcados e n�o marcados
    //Busca a rota mais curta para os n�s orN e deN;
    //Inicializa��o do algoritmo:
    for(i = 0; i < Def::getNnodes(); i++)
    {
        if(i != orN)
        {
            CustoVertice[i] = Def::MAX_DOUBLE;
            HopsVertice[i] = Def::MAX_INT;
            for(s = 0; s < Def::getSE(); s++)
                DispVertice[i*Def::getSE() + s] = false; //O slot � assumido indispon�vel
        }
        else
        {
            CustoVertice[i] = 0.0;
            HopsVertice[i] = 0;
            for(s = 0; s < Def::getSE(); s++)
                DispVertice[i*Def::getSE() + s] = true; //O slot � assumido dispon�vel
        }
        Precedente[i] = -1;
        Status[i] = 0; //N� ainda n�o foi marcado
    }
    //Processo de Dijkstra:
    VA = Def::getNnodes();
    while(VA > 0)
    {
        //Toma o n� de menor custo ainda n�o marcado
        min = Def::MAX_DOUBLE;
        for(i = 0; i < Def::getNnodes(); i++)
            if((Status[i] == false)&&(CustoVertice[i] < min))
            {
                min = CustoVertice[i];
                k = i; //N� k � o de menor custo
            }
        Status[k] = true; //N� k passa a ser marcado
        //A partir do n� k, atualiza ou n�o o custo de seus n�s vizinhos (j)
        VA = VA-1;
        for(j = 0; j < Def::getNnodes(); j++)
            if((Status[j] == false)&&(Topology[k*Def::getNnodes()+j] != 0)) //O n� j � n�o marcado e vizinho do n� k
            {
                //Calcula O vetor de disponibilidade em j por uma rota proveniente de k
                for(s = 0; s < Def::getSE(); s++)
                    DispAux[s] = DispVertice[k*Def::getSE() + s] * !Topology_S[s*Def::getNnodes()*Def::getNnodes() + k*Def::getNnodes() + j];
                //Calcula o n�mero de hops em j por uma rota proveniente de k
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
        vetDisp[s] = checkDisp(route, s);//Checa a disponibilidade no caminho 'path' para o slot s;
    //Carrega vetSlotsUsed com o n�mero de enlaces ocupados em cada slot;
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
        if(fit == true && soma == NslotsReq)  //A requisi��o se encaixa entre os slots s,s+1,...,s+NslotsReq-1
        {
            somaAlocacao++; // verifica quantas posi��es est�o dispon�veis para aloca��o
            vetAloc[s] = 1; //posso alocar a partir desta posi��o
        }
    }
    // fazer o sorteio do slot que ser� alocada
    if(somaAlocacao > 0) // se existir pelo menos um conjunto slot dispon�vel com o tamanho da solicita��o
    {
        alocarSlot = rand()% somaAlocacao; // fazer aloca��o no conjunto de slot dispov�vel "alocarSlot"
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
    NslotsUsed = 0; //Valores n�o permitidos
    int sum;
    for(int s = 0; s <= Def::getSE() - NslotsReq; s++)
    {
        sum = 0;
        for(int se = s; se < s + NslotsReq; se++)
        {
            if(checkDisp(route, se))
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
        vetDisp[s] = checkDisp(route, s);//Checa a disponibilidade no caminho 'path' para o slot s;
    //Carrega vetSlotsUsed com o n�mero de enlaces ocupados em cada slot;
    int soma;
    for(int s = 0; s < Def::getSE(); s++)
    {
        if(vetDisp[s] == true)
            vetSlotsUsed[s] = sumOccupation(s);
        else
            vetSlotsUsed[s] = -1;
    }
    //Obtem o slot mais ocupado dentre os que podem receber a requisi��o
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
        if(fit == true && soma > maxSoma)  //A requisi��o se encaixa entre os slots s,s+1,...,s+NslotsReq-1 e nesta posi��o haver� a maior soma de enlaces ocupados na rede;
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
	//Obtem quais slots podem come�ar a requisi��o:
	bool *vetDispFitSi = new bool[Def::getSE()];
    for(s = 0; s <= Def::getSE() - NslotsReq; s++)
		vetDispFitSi[s] = checkFitSi(vetDisp, s, NslotsReq); //Se a conex�o pode ser inserida em s, s+1,...,s+NslotsReq-1
	//Calcular a disponibilidade de cada caminho que interfere com path
	int H = Route[path*(N+1)]; //H = n�mero de hops da requisi��o
	int r, path_int, L_or, L_de;
	bool vetDispInt[Def::getSE()];
	vector<bool> *RouteDisp = new vector<bool>[RouteInt[path]->size()];
	for(r = 0; r < RouteInt[path]->size(); r++){
		path_int = RouteInt[path]->at(r); //r_int � o �ndice da rota que interfere com a rota de �ndice r
		H = Route[path_int*(N+1)]; //H = n�mero de hops da conex�o
		for(s = 0; s < Def::getSE(); s++){
			RouteDisp[r].push_back(true);
			for(int c = 1; c <= H; c++)	{
				L_or = Route[path_int*(N+1)+c];
				L_de = Route[path_int*(N+1)+c+1];
				if(Topology_S[s*Def::getNnodes()*Def::getNnodes() + L_or*Def::getNnodes() + L_de] == true){ //O slot s est� ocupado no enlace L_or->L_de
					RouteDisp[r].at(s) = false;
					break;
				}
			}
		}
	}
	//Calcular a perda de cada possibilidade de aloca��o:
	int vetCapInic[Def::getSE()+1], vetCapFin[Def::getSE()+1];
	double perda, perdaMin = MAX_DOUBLE;
	for(s = 0; s <= Def::getSE() - NslotsReq; s++)
		if(vetDispFitSi[s] == true){ //O caminho pode ser iniciado neste slot
			perda = 0.0;
			//Obter as rotas interferentes
			for(r = 0; r < RouteInt[path]->size(); r++){
				path_int = RouteInt[path]->at(r); //r_int � o �ndice da rota que interfere com a rota de �ndice r
				//Carrega DispInt com a disponibilidade inicial da rota de �ndice r_int:
				for(int se = 0; se < Def::getSE(); se++)
					vetDispInt[se] = RouteDisp[r].at(se);
				//Calcula a capacidade de aloca��o nesta rota:
				calcVetCap(vetDispInt, vetCapInic);
				//Assume a aloca��o da rota de �ndice r nos slots s,s+1,...,s+NslotsReq-1 => retira os slots s,s+1,...,s+NslotsReq-1 de vetDispInt
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
        if(s <= Def::getSE()-NslotsReq)  //si � capaz de suportar a requisi��o;
        {
            sum = 0;
            for(int se = s; se < s + NslotsReq; se++)
                if(checkDisp(route, se))
                    sum++;
                else
                    break;
        }
        if(sum == NslotsReq)  //O slot s pode atender a requisi��o
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
bool checkDisp(const Route *route, int s)
{
    int L_or, L_de;
    for(unsigned c = 0; c < route->getNhops(); c++)
    {
        L_or = route->getNode(c);
        L_de = route->getNode(c+1);
        if(Topology_S[s*Def::getNnodes()*Def::getNnodes() + L_or*Def::getNnodes() + L_de] == true) // o enlace c->c+1 esta ocupado;
            return false;
    }
    return true;
}

// -------------------------------------------------------------------------- //
bool checkFitSi(const bool* vetDisp, int s, int NslotsReq)  //Esta fun��o indica se a conex�o pode ser inserida em si,si+1,...,si+NslotsReq
{
    assert(s + NslotsReq - 1 < Def::getSE());
    for(int b = s; b < s + NslotsReq; b++)
        if(vetDisp[b] == false)
            return false;
    return true;
}


// FIM //
