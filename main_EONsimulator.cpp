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
#include "Main_Auxiliar.h"
#include "Conexao.h"
#include "Def.h"
#include "General.h"
#include "Heuristics.h"
#include "Route.h"
#include "Node.h"
#include "Enlace.h"
#include "ASE_Noise.cpp"
#include "Metricas.cpp"
#include "RWA.cpp"

using namespace std;

//Protótipos de Funções
void AccountForBlocking(int NslotsReq, int NslotsUsed, int nTaxa); /*Realiza ações necessárias para quando uma conexão foi bloqueada*/
void AccountForBlockingOSNR(int NslotsReq, int NslotsUsed); /*Realiza ações necessárias para quando uma conexão foi bloqueada*/
long double AvaliarOSNR(const Route *Rota, int NSlotsUsed); /*avalia a ONSR da routa passada como parâmetro*/
bool checkFitSi(const bool* vetDisp, int s, int NslotsReq); /*Indica se a conexao pode ser inserida em [s:s+NslotsReq]*/
void clearMemory(); /*Limpa e zera todas as constantes de Def.h, reinicia o tempo de simulação e libera todos os slots.*/
void CompressCon(Event*); /*Diminui a quantidade de slots reservados para a conexão. Pode ser configurada para retirar sempre o slot da direita ou o da esquerda ou o da direita, escolhido aleatoriamente. Lembrar de conferir isto.*/
void CompressRandom(Conexao *con); /*Comprime conexão, removendo slot da esquerda ou da direita*/
void CompressRight(Conexao *con); /*Comprime conexão, removendo o slot da direita*/
void createStructures(); /*Cria estrutura topológica da rede, e carrega os dados de Topology.txt*/
void DefineNextEventOfCon(Event* evt); /*Define se o próximo evento da conexão será uma expansão, compressão ou desativação da conexão*/
void ExpandCon(Event*); /*Exprime conexão, inserindo um novo slot disponível para a mesma*/
bool FillSlot(const Route* route, const int s, const bool b); /*Preenche todos os slots s da rota route com o valor b (ou seja, ocupa ou livra o slot s de todos os enlaces da rota)*/
void GrauDosNodes(void); /*Calcula o grau dos nós*/
void Load(); /*Função que lê os dados relativos à simulação. Realiza tarefas de io. Verificar significado de várias variáveis em seu escopo*/
void RefreshNoise(); /*atualiza os ruídos dos enlaces*/
bool ReleaseSlot(const Route* route, int s); /*Libera o slot s em todos os enlaces da Rota route*/
void RemoveCon(Event*); /*Retira uma conexão da rede - liberando todos os seus slots*/
void RequestCon(Event*); /*Cria uma conexão. Dados dois nós, procura pelo algoritmo de roteamento definido uma rota entre os mesmos. Após encontrar a rota, cria a conexão, e por fim agenda o próximo evento de requisição de conexão.*/
void ScheduleEvent(Event*); /*Programa evento para execução, criando a fila*/
void SDPairReq(int &orN, int &deN); /*cria um par de nó origem e destino, aleatoriamente*/
void setReqEvent(Event*, TIME); /*Cria um evento de requisição a partir do instante de criação (TIME)*/
void Sim(); /*Define parâmetros anteriores à simulação. Escolher aqui como o tráfego é distribuído entre os slots e a heurística que será utilizada*/
void SimCompFFO(); /*Simula testando as diversas heurísticas. Usa tráfego aleatoriamente distribuído. Descomentar linha em main() para usar esse código*/
void Simulate(); /*Função principal. Inicia a simulação, chamando clearMemory(). Então começa a fazer as requisições de acordo com o tipo de Evento que ocorreu, até que a simulação termine.*/
void Simulate_dAMP(); /*Análogo de Simulate(), mas para variações na distância entre os amplificadores*/
int SlotsReq(int Ran, Event *evt); /*coverte a taxa em um número de slots.*/
int TaxaReq();  /*gera um número aleatório, sob uma distribuição uniforme, que representará a taxa de transmissão que a requisição solicitará.*/
void TryToConnect(const Route* route, const int NslotsReq, int& NslotsUsed, int& si); /*Tenta alocar na rota route um número NslotsReq de slots. O Algoritmo de Alocação é relevante aqui. Retorna si, o slot inicial (-1 se não conseguiu alocar) e NslotsUsed (número de slots que conseguiu alocar).*/

int main() {
    Load();
    cout << "Inicio da simulacao:" << endl;
    createStructures();
    if (Alg_Routing == DJK)
        Dijkstra();
    else if (Alg_Routing == SP)
        DijkstraSP();
    if (escSim == Sim_PbReq) {
        for(laNet = LaNetMin; laNet <= LaNetMax; laNet += LaPasso) {
            Sim();
            //SimCompFFO(); Simula usando as listas FF otimizadas
        }
    } else if (escSim == Sim_OSNR) {
        //Simulacao para varios trafegos
        cout << "Insira a carga da rede." << endl;
        cin >> laNet;
        for(long double osnr = OSNRMin; osnr <= OSNRMax; osnr += OSNRPasso) {
            Def::setOSNR(osnr);
            Sim();
            //SimCompFFO(); Simula usando as listas FF otimizadas
        }
    } else if (escSim == Sim_DAmp) {
        DijkstraSP();
        cout << "Limiar: " << Def::getlimiarOSNR(_4QAM, 100E9) << "dB" << endl;
        for(long double osnr = OSNRMin; osnr <= OSNRMax; osnr += OSNRPasso) {
            Def::setOSNR(osnr);
            for (long double dAmplif = DAmpMin; dAmplif <= DAmpMax; dAmplif += DAmpPasso) {
                Def::set_DistaA(dAmplif);
                RefreshNoise();
                Simulate_dAMP();
            }
        }
    }

    delete []Topology;
    delete []Topology_S;
    delete []AllRoutes;
    delete []Caminho;
    if (FFlists != NULL) delete FFlists;
    cout << "Fim do programa";
    cin.get();
    cin.get();
    return 0;
}

void AccountForBlocking(int NslotsReq, int NslotsUsed, int nTaxa) {
    if(NslotsUsed <= 0) { //A conexao foi bloqueada
        Def::numReq_Bloq++;
        Def::numReqBloq_Taxa[nTaxa]++;
    }
    Def::numSlots_Bloq += (NslotsReq - NslotsUsed);
}

bool checkFitSi(const bool* vetDisp, int s, int NslotsReq) {
    assert(s + NslotsReq - 1 < Def::getSE());
    for(int b = s; b < s + NslotsReq; b++)
        if(vetDisp[b] == false)
            return false;
    return true;
}

void clearMemory() {
    int s, L_or, L_de;
    Conexao *con;
    Event *evtPtr;
    const Route* route;
    Def::numReq = Def::numReq_Bloq = Def::numSlots_Req = Def::numSlots_Bloq = Def::numHopsPerRoute = Def::netOccupancy = simTime = Def::numReq_BloqPorOSNR = Def::numSlots_BloqPorOSNR = 0.0;
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
                    Topology_S[s][L_or][L_de] = false;
            }
            delete con;
        }
        evtPtr = firstEvent;
        firstEvent = firstEvent->nextEvent;
        delete evtPtr;
    }

    for (int i = 0; i<Def::get_numPossiveisTaxas(); i++) {
        Def::numReqBloq_Taxa[i] = 0;
        Def::numReq_Taxa[i] = 0;
        Def::tempoTotal_Taxa[i] = 0;
    }

    //Checar se limpeza foi realizada corretamente
    for(int orN = 0; orN < Def::getNnodes(); orN++)
        for(int deN = 0; deN < Def::getNnodes(); deN++)
            for(s = 0; s < Def::getSE(); s++)
                assert(Topology_S[s][orN][deN] == false);
    assert(firstEvent == NULL);
}

void CompressCon(Event* evt) {
    if (ExpComp) {
        //Remover um slot lateral
        Conexao *con = evt->conexao;
        CompressRight(con); //Remove sempre o slot da direita. Tentar outras heuristicas
        //CompressRandom(con);
        assert(con->getLastSlot() >= con->getFirstSlot()); //Apenas para checar erros
        DefineNextEventOfCon(evt);
        ScheduleEvent(evt);
    }
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

void createStructures() {
    Topology = new long double*[Def::getNnodes()]; //matriz de conexões entre nós
    for (int i=0 ; i < Def::getNnodes() ; i++) Topology[i] = new long double[Def::getNnodes()];
    Topology_S = new bool**[Def::getSE()]; //matriz de ocupação de slots de cada enlace
    for (int i=0 ; i < Def::getSE() ; i++) {
        Topology_S[i] = new bool*[Def::getNnodes()];
        for (int j=0; j < Def::getNnodes() ; j++) {
            Topology_S[i][j] = new bool[Def::getNnodes()];
            for (int k = 0; k < Def::getNnodes(); k++)
                Topology_S[i][j][k] = false;
        }
    }

    //Carrega topologia de rede a partir do arquivo Topology.txt
    AllRoutes = new vector<Route*>[Def::getNnodes()*Def::getNnodes()];
    int orN, deN;
    for (orN = 0; orN < Def::getNnodes(); orN++) {
        for(deN = 0; deN < Def::getNnodes(); deN++) {
            switch(escTop) {
                case PacificBell: Topol2 >> Topology[orN][deN]; break;
                case NSFNet: Topol>>Topology[orN][deN]; break;
                case NFSNetMod: Topol5>>Topology[orN][deN]; break;
                case PontoaPonto8: Topol3>>Topology[orN][deN]; break;
                case PontoaPonto4: Topol4>>Topology[orN][deN]; break;
                case Top1: Topol6>>Topology[orN][deN]; break;
                case Top2: Topol7>>Topology[orN][deN]; break;
            }
            cout<<Topology[orN][deN]<<" ";
        }
        cout << endl;
    }

    //Calcula o grau de cada no
    GrauDosNodes();

    for (int i=0;i < Def::getNnodes() ; i++) {
        Rede.push_back(Node(i));
    }

    //Implemente os Enlaces
    Caminho = new vector<Enlace>[Def::getNnodes()];
    for (int i=0; i < Def::getNnodes(); i++){
        for(int j=0; j < Def::getNnodes(); j++){
            double distancia_temp;
            switch(escTop) {
                case PacificBell: Topol2 >> distancia_temp; break;
                case NSFNet: Topol>>distancia_temp; break;
                case NFSNetMod: Topol5>>distancia_temp; break;
                case PontoaPonto8: Topol3>>distancia_temp; break;
                case PontoaPonto4: Topol4>>distancia_temp; break;
                case Top1: Topol6>>distancia_temp; break;
                case Top2: Topol7>>distancia_temp; break;
            }
            if(Topology[i][j] == 1){
                Caminho[i].push_back(Enlace(&Rede.at(i),&Rede.at(j),distancia_temp));
            } else {
                Caminho[i].push_back(Enlace(NULL,NULL,Def::MAX_INT));
            }
        }
    }
}

void DefineNextEventOfCon (Event* evt) {
    //Recalcula Te (tempo de expansao) e Tc (tempo de compressao) e os compara com o tempo para desativacao
    //So pode haver expansao se o numero de slots for menor do que Def::getSE()

    Conexao *con = evt->conexao;
    TIME Te, Tc, evtTime;
    EventType evtType = UNKNOWN;
    if ( (ExpComp == true) && ((con->getLastSlot() - con->getFirstSlot() + 1) < Def::getSE()) )
        //Se o numero de slots utilizados for menor do que Def::getSE()
        Te = simTime + General::exponential(laE);
    else
        Te = Def::MAX_DOUBLE;
        //So pode haver compressao se o numero de slots for maior ou igual a 2, pois 1 e o numero minimo do slots que podem estar ativos para uma conexao

    if ( (ExpComp == true) && (con->getLastSlot() - con->getFirstSlot() > 0) )
        Tc = simTime + General::exponential(muC);
    else
        Tc = Def::MAX_DOUBLE;
        //Checa o proximo evento dentre expansao, compressao e desativacao
    if(Te < Tc) {
        if(Te < con->getTimeDesc()) {
            evtTime = Te;
            evtType = Exp;
        } else {
            evtTime = con->getTimeDesc();
            evtType = Desc;
        }
    } else {
        if(Tc < con->getTimeDesc()) {
            evtTime = Tc;
            evtType = Comp;
        } else {
            evtTime = con->getTimeDesc();
            evtType = Desc;
        }
    }
    evt->time = evtTime;
    evt->type = evtType;
}


void ExpandCon(Event* evt) {
    if (ExpComp) {
        Conexao *con = evt->conexao;
        //Procura um slot lateral. Assume-se que tenta primeiro o slot a esquerda e depois o a direita.
        if(con->getFirstSlot() > 0 && CheckSlotAvailability(con->getRoute(), con->getFirstSlot()-1)) {
            FillSlot(con->getRoute(), con->getFirstSlot()-1, true);
            con->incFirstSlot(-1);
        } else if(con->getLastSlot()<Def::getSE()-1 && CheckSlotAvailability(con->getRoute(),con->getLastSlot()+1)) {
            FillSlot(con->getRoute(), con->getLastSlot()+1, true);
            con->incLastSlot(+1);
        } else
            Def::numSlots_Bloq++;
        Def::numSlots_Req++;
        DefineNextEventOfCon(evt);
        ScheduleEvent(evt);
    }
}

bool FillSlot(const Route* route, const int s, const bool b) {
    int L_or, L_de;
    for (unsigned c = 0; c < route->getNhops(); c++) {
        L_or = route->getNode(c);
        L_de = route->getNode(c+1);
        assert(Topology_S[s][L_or][L_de] != b);
        Topology_S[s][L_or][L_de] = b;
    }
    return true;
}

void GrauDosNodes() {
    int node_temp = 0;
    Def::clearGrauNo();
    for (int orN = 0; orN < Def::getNnodes() ; orN++) {
        for (int deN = 0; deN < Def::getNnodes() ; deN++) {
            if (Topology[orN][deN] == 1) node_temp++;
        }
        Def::setGrauNo(node_temp);
        node_temp=0;
    }
}

void Load() {
    Def::setPossiveisTaxas();

    int Npontos, aux;
    long double op;

    cout << "Escolha a Simulação. " << endl << "\tProbabilidade de Bloqueio <" << Sim_PbReq << ">;" << endl << "\tOSNR <" << Sim_OSNR << ">; " << endl << "\tDistancia dos Amplificadores <" << Sim_DAmp << ">." << endl;
    cin >> aux;
    escSim = (Simulacao)aux;

    cout << "Escolha a topologia." << endl << "\tPacific Bell <" << PacificBell << ">; "<< endl << "\tNSFNet <" << NSFNet << ">; " << endl << "\tNSFNet Modificada (Reduzida) <" << NFSNetMod << ">;" << endl << "\tPonto a Ponto de 4 Nós <" << PontoaPonto4 <<">; "  << endl << "\tPonto a Ponto de 8 Nós <" << PontoaPonto8 << ">; " << endl << "\tTop1 <" << Top1 << ">;" << endl << "\tTop2 <" << Top2 << ">;" << endl;
    cin>>aux;
    escTop = (Topologia)aux;

    //Adquire o numero de Nos:
    switch (escTop) {
        case PacificBell: Topol2 >> aux; break;
        case NSFNet: Topol>>aux; break;
        case NFSNetMod: Topol5>>aux; break;
        case PontoaPonto4: Topol4>>aux; break;
        case PontoaPonto8: Topol3>>aux; break;
        case Top1: Topol6>>aux; break;
        case Top2: Topol7>>aux; break;
    }

    Def::setNnodes(aux);
    cout << "Numero de nos: "<< Def::getNnodes() << endl;

    cout<<"Considera a OSNR? <"<<SIM<<"> Sim ou <"<<NAO<<"> Nao"<<endl;
    cin>>aux;
    AvaliaOsnr = (Respostas)aux;
    cout << AvaliaOsnr << endl;

    cout << "Entre com a banda de um slot, em GHz (valores comuns sao 1.5625, 3.125, 6.25 e 12.5)" << endl;
    cin >> op;
    Def::setBslot(op);
    Def::setBref(12.5);

    switch (escTop) {
        case PacificBell: Topol2 >> aux; break;
        case NSFNet: Topol>>aux; break;
        case NFSNetMod: Topol5>>aux; break;
        case PontoaPonto4: Topol4>>aux; break;
        case PontoaPonto8: Topol3>>aux; break;
        case Top1: Topol6>>aux; break;
        case Top2: Topol7>>aux; break;
    }
    Def::setSE(aux); //o enlace tem 100GHz de banda
    cout << "Numero de Slots por Enlace: " << Def::getSE() << endl;

    //Outras entradas para o simulador
    Def::setSR(Def::getSE()); //Uma requisicao nao podera pedir mais que SE slots

    cout << "\t" << DJK<<" - DJK \n\t"<<DJK_Formas<<" - DJK_Formas \n\t"<< DJK_Acum<<" - DijkstraAcumulado\n\t" << SP << " - Shortest Path\n\t"<< DJK_SPeFormas << " - DJK Shortest Path e Formas" << endl;
    cout << "Entre com o Algoritmo de Roteamento: ";
    cin >> Alg_Routing;

    cout<<"\t" << RD<<" - Random \n\t"<<FF<<" - FirstFit \n\t"<<MU<<" - Most Used \n\t"<<FFO<<" - FirstFitOpt "<<endl;
    cout << "Entre com o Algoritmo de Alocacao: ";
    cin >> Alg_Aloc;

    //Cria as listas First Fit
    if(Alg_Aloc == FFO || Alg_Aloc == FF) {
        FFlists = new vector<int>*[Def::getSR()+1];
        for(int i = 0; i < Def::getSR()+1; i++)
            FFlists[i] = new vector<int>(0);
    }

    cout <<"Entre com o mu (taxa de desativacao de conexoes): ";
    cin >> mu; //mu = taxa de desativacao das conexoes;
    if (escSim == Sim_OSNR || escSim == Sim_DAmp) {
        cout << "Entre com..." << endl;
        cout << "OSNR minimo = ";
        cin >> OSNRMin; // La = taxa de chegada das conexoes;
        cout <<"OSNR maximo = ";
        cin >> OSNRMax;
        cout<<"#Pontos no grafico = ";
        cin >> Npontos;
        OSNRPasso = (OSNRMax-OSNRMin)/(Npontos-1);
    }
    if (escSim == Sim_PbReq) {
        cout << "La = Taxa de Chegada de Conexoes. Entre com..." << endl;
        cout << "LaNet minimo = ";
        cin >> LaNetMin; // La = taxa de chegada das conexoes;
        cout <<"LaNet maximo = ";
        cin >> LaNetMax;
        cout<<"#Pontos no grafico = ";
        cin >> Npontos;
        LaPasso = (LaNetMax-LaNetMin)/(Npontos-1);
    }
    if (escSim == Sim_DAmp) {
        cout << "Entre com..." << endl;
        cout << "Distancia minima entre Amplf. de Linha = ";
        cin >> DAmpMin;
        cout <<"Distancia maxima entre Amplf. de Linha = ";
        cin >> DAmpMax;
        cout<<"#Pontos no grafico = ";
        cin >> Npontos;
        DAmpPasso = (DAmpMax-DAmpMin)/(Npontos-1);
    }

    if (AvaliaOsnr==SIM) {
        cout << "Entre com a potencia de entrada, em dBm." << endl;
        cin>>op;
        Def::set_Pin(op);
        cout << "Entre com a potencia de referencia da fibra, em dBm." << endl;
        cin>>op;
        Def::set_Pref(op);
        if (escSim != Sim_DAmp) {
            cout<<"Entre com distancia entre os amplificadores"<<endl;
            cin>>op;
            Def::set_DistaA(op);
        }
        cout<<"Se a arquitetura for Brodcasting and Select digite 1. Se for Switching and Select digite 2."<<endl;
        cin>>aux;
        if (aux == 1)
            Def::set_Arquitetura(Def::BS);
        else if (aux == 2)
            Def::set_Arquitetura(Def::SS);
    }

    //Dados para a expansao e compressao das conexoes:
    cout << "Considerar expansao e compressao do numero de subportadoras das Conexoes? <0> Falso; <1> Verdadeiro;"<<endl;
    cin >> ExpComp;
    if(ExpComp) {
        cout<<"Entre com o muC: ";
        cin>> muC;
        cout<<"Entre com o LaE: ";
        cin >> laE;
    }

    long double nR;
    if (escSim != Sim_DAmp) {
        cout<<"Entre com o numero maximo de requisicoes que o programa vai rodar antes de terminar: ";
        cin>> nR;
        Def::setNumReqMax(nR);
    }

    for(int f = 0; f <= Def::getSE()-Def::getSR()+1; f++)
        Metrica<<f<<"\t"<<1.0/(f+1)<<endl;
}

void RefreshNoise() {
    for (int i = 0; i< Def::getNnodes(); i++ ) {
        for (int j = 0; j < Def::getNnodes(); j++ ) {
            Caminho[i].at(j).recalcular();
        }
    }
}

bool ReleaseSlot(const Route* route, int s) {
    int L_or, L_de;
    for(unsigned c = 0; c < route->getNhops(); c++) {
        L_or = route->getNode(c);
        L_de = route->getNode(c+1);
        assert(Topology_S[s][L_or][L_de] == true);
        Topology_S[s][L_or][L_de] = false;
    }
    return true;
}

void RemoveCon(Event* evt) {
    for(int s = evt->conexao->getFirstSlot(); s <= evt->conexao->getLastSlot(); s++) {
        assert( (s >= 0) && (s < Def::getSE()) );
        ReleaseSlot(evt->conexao->getRoute(), s);
    }
    delete evt->conexao;
    delete evt;
}

void RequestCon(Event* evt) {
    int orN, deN, NslotsReq, NslotsUsed, si, nTaxa;
    SDPairReq(orN, deN);
    nTaxa = TaxaReq();
    if (escSim == Sim_DAmp) {
        nTaxa = Def::get_numPossiveisTaxas() - 1;
    }

    //Para o conjunto de rotas fornecida pelo roteamento, tenta alocar a requisicao:
    Route *route;
    long double OSNR = 0;

    Def::numReq++;
    Def::numReq_Taxa[nTaxa]++;

    EsquemaDeModulacao Esquemas[numEsquemasDeModulacao] = { _64QAM, _16QAM, _4QAM };
    for (int Esq = 0; Esq < numEsquemasDeModulacao; Esq++) {
        evt->Esquema = Esquemas[Esq];
        NslotsReq = SlotsReq(nTaxa, evt);

        if(Alg_Routing == DJK_Formas)
            DijkstraFormas(orN, deN, NslotsReq);
        if(Alg_Routing == DJK_Acum)
            DijkstraAcum(orN, deN, NslotsReq);
        if(Alg_Routing == DJK_SPeFormas)
            DijkstraSPeFormas(orN,deN,NslotsReq);

        for(unsigned int i = 0; i < AllRoutes[orN*Def::getNnodes()+deN].size(); i++) {
            route = AllRoutes[orN*Def::getNnodes()+deN].at(i); //Tenta a i-esima rota destinada para o par orN-deN
            NslotsUsed = 0;
            si = -1;
            TryToConnect(route, NslotsReq, NslotsUsed, si);
            assert( (NslotsUsed == 0) || (NslotsUsed == NslotsReq) ); //Tirar isso aqui quando uma conexao puder ser atendida com um numero menor de slots que o requisitado
            if(NslotsUsed > 0) { //A conexao foi aceita
                assert(NslotsUsed <= NslotsReq && si >= 0 && si <= Def::getSE()-NslotsUsed);
                if (AvaliaOsnr==SIM) OSNR = AvaliarOSNR(route,NslotsUsed);
                if (AvaliaOsnr==NAO || OSNR >= Def::getlimiarOSNR(evt->Esquema, Def::PossiveisTaxas[nTaxa])) { //aceita a conexao
                //Inserir a conexao na rede
                    int L_or, L_de;
                    for(unsigned c = 0; c < route->getNhops(); c++) {
                        L_or = route->getNode(c);
                        L_de = route->getNode(c+1);
                        for(int s = si; s < si + NslotsUsed; s++) {
                            assert(Topology_S[s][L_or][L_de] == false);
                            Topology_S[s][L_or][L_de] = true;
                            //Os slots sao marcados como ocupados
                        }
                    }

                    Def::numHopsPerRoute += route->getNhops();
                    Def::netOccupancy += NslotsUsed*route->getNhops();

                    //Cria uma nova conexao
                    long double Tempo = General::exponential(mu);
                    Conexao *newConexao = new Conexao(route, si, si + NslotsUsed - 1, simTime + Tempo);
                    //Agendar um dos eventos possiveis para conexao (Expandir, contrair, cair, etc):
                    Event *evt = new Event;
                    evt->conexao = newConexao;
                    DefineNextEventOfCon(evt);
                    ScheduleEvent(evt);
                    Def::tempoTotal_Taxa[nTaxa] += Tempo;
                    break;
                } else { //conexao bloqueada por OSNR
                    NslotsUsed = 0;
                    if (Esq == numEsquemasDeModulacao - 1) {
                        AccountForBlockingOSNR(NslotsReq,NslotsUsed);
                    }
                }
            }
        }
        if (NslotsUsed != 0) break;
    }
    Def::numSlots_Req += NslotsReq;

    //Verifica quantas conexoes e quantos slots foram bloqueados
    AccountForBlocking(NslotsReq, NslotsUsed, nTaxa);
    //Define o novo evento de chegada de requisicao
    long double IAT = General::exponential(laNet); //Inter-arrival time
    setReqEvent(evt, simTime + IAT);
    assert(evt->type == Req);
    ScheduleEvent(evt); //Reusa este mesmo objeto evt
}

void ScheduleEvent(Event *evt) {
    Event *evtAux = firstEvent, *evtAnt = NULL;
    while(evtAux != NULL) {
        if(evt->time < evtAux->time)
            break;
        else {
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

void SDPairReq(int &orN, int &deN) {
    orN = rand()% Def::getNnodes();
    deN = rand()% (Def::getNnodes()-1);
    if(deN >= orN)
        deN += 1;
    if( (orN < 0) || (orN>= Def::getNnodes()) || (deN<0) || (deN>=Def::getNnodes()) ||(deN == orN)) {
        cout<<"Erro em SDPair";
        cin.get();
    }
}

void setReqEvent(Event* evt, TIME t) {
    evt->time = t;
    evt->type = Req;
    evt->nextEvent = NULL;
    evt->conexao = NULL;
    if (escSim == Sim_DAmp) {
        evt->Esquema = _4QAM;
    }
}

void Sim() {
    //Indica como o trafego e distribuido entre s = 1, 2, ..., SE
    Def::setLaUniform(laNet);
    //Def::setLaRandom(laNet);

    if(Alg_Aloc == FFO) {
        //Indica uma Heuristica:
        Heuristics::FFO_convencional(FFlists);
        //Heuristics::FFO_extremos(FFlists);
        //Heuristics::FFO_invertido(FFlists);
        //Heuristics::FFO_metrica(FFlists);
    }
    Simulate();
}

void SimCompFFO() {
    long double Pb_conv, Pb_ext, Pb_met;
    long double difPerc, difPerc_FFOmet_FFOext_Pos = 0.0, difPerc_FFOmet_FFOext_Neg = 0.0,	difPerc_FFOconv_FFOext_Pos = 0.0, difPerc_FFOconv_FFOext_Neg = 0.0;
    for(int it = 0; it < 100; it++) {
        Def::setLaRandom(laNet);

        //Testa as heuristicas
        Heuristics::FFO_convencional(FFlists);
        Simulate();
        Pb_conv = Def::numReq_Bloq/Def::numReq;

        Heuristics::FFO_extremos(FFlists);
        Simulate();
        Pb_ext = (double)Def::numReq_Bloq/Def::numReq;

        Heuristics::FFO_metrica(FFlists);
        Simulate();
        Pb_met = Def::numReq_Bloq/Def::numReq;

        //Calcular as diferencas percentuais:
        //Entre FFOmetrica e FFOextremos
        if( (Pb_met > 1000*(1.0/Def::numReq)) && (Pb_ext > 1000*(1.0/Def::numReq)) ) {
            //Tenho confianca nos resultados
            difPerc = (Pb_met - Pb_ext)/Pb_ext;
            if(difPerc > difPerc_FFOmet_FFOext_Pos) {
                difPerc_FFOmet_FFOext_Pos = difPerc;
                for(int Lr = 1; Lr <= Def::getSR(); Lr++)
                    ResulFFOmet_FFOext<<Def::getLaNet(Lr)<<" ";
                ResulFFOmet_FFOext<<endl;
            }
            if(difPerc < difPerc_FFOmet_FFOext_Neg) {
                difPerc_FFOmet_FFOext_Neg = difPerc;
                for(int Lr = 1; Lr <= Def::getSR(); Lr++)
                    ResulFFOext_FFOmet<<Def::getLaNet(Lr)<<" ";
                ResulFFOext_FFOmet<<endl;
            }

            difPerc = (Pb_conv - Pb_ext)/Pb_ext;
            if(difPerc > difPerc_FFOconv_FFOext_Pos) {
                difPerc_FFOconv_FFOext_Pos = difPerc;
                for(int Lr = 1; Lr <= Def::getSR(); Lr++)
                    ResulFFOconv_FFOext<<Def::getLaNet(Lr)<<" ";
                ResulFFOconv_FFOext<<endl;
            }
            if(difPerc < difPerc_FFOconv_FFOext_Neg) {
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

void Simulate() {
    clearMemory();
    //Cria o primeiro evento da rede como uma requisicao:
    firstEvent = new Event;
    setReqEvent(firstEvent, simTime);
    while(Def::numReq < Def::getNumReqMax()) {
        Event *curEvent = firstEvent;
        firstEvent = firstEvent->nextEvent;
        simTime = curEvent->time;
        if(curEvent->type == Req) {
            RequestCon(curEvent);
        } else if(curEvent->type == Desc) {
            RemoveCon(curEvent);
        } else if(curEvent->type == Exp) {
            assert(ExpComp); //Um evento deste tipo so pode ocorrer se ExpComp=true;
            ExpandCon(curEvent);
        } else if(curEvent->type == Comp) {
            assert(ExpComp); //Um evento deste tipo so pode ocorrer se ExpComp=true;
            CompressCon(curEvent);
        }
    }

    cout <<"Simulation Time= " << simTime << "  numReq=" << Def::numReq << endl;
    if (escSim == Sim_PbReq) {
        cout << "nu0= " << laNet << "   PbReq= " << ProbBloqueio() << "   PbAc= " << ProbAceitacao() << "   PbSlots= " << (long double) Def::numSlots_Bloq/Def::numSlots_Req << " HopsMed= " << (long double) Def::numHopsPerRoute/(Def::numReq-Def::numReq_Bloq) << " netOcc= " << (long double) Def::netOccupancy << endl;
        Resul << laNet << "\t" << (long double) Def::numReq_Bloq/Def::numReq << "\t" << (long double) (1.0 - Def::numReq_Bloq/Def::numReq) << "\t" << (long double) Def::numSlots_Bloq/Def::numSlots_Req << "\t" << (long double) Def::numHopsPerRoute/(Def::numReq-Def::numReq_Bloq) << "\t" << Def::netOccupancy << endl;
        ResulOSNR << laNet << "\t" << Def::numReq_BloqPorOSNR/Def::numReq_Bloq << endl;
    } else if (escSim == Sim_OSNR) {
        cout << "OSNR = " << Def::get_OSRNin() << "   PbReq= " << ProbBloqueio() << "   PbAc= " << ProbAceitacao() << "   PbSlots= " << (long double) Def::numSlots_Bloq/Def::numSlots_Req << " HopsMed= " << (long double) Def::numHopsPerRoute/(Def::numReq-Def::numReq_Bloq) << " netOcc= " << (long double) Def::netOccupancy << endl;
        Resul << Def::get_OSRNin() << "\t" << (long double) Def::numReq_Bloq/Def::numReq << "\t" << (long double) Def::numSlots_Bloq/Def::numSlots_Req << "\t" << (long double) Def::numHopsPerRoute/(Def::numReq-Def::numReq_Bloq) << "\t" << Def::netOccupancy << endl;
        ResulOSNR << Def::get_OSRNin() << "\t" << Def::numReq_BloqPorOSNR/Def::numReq_Bloq << endl;
    }

    {
        ProbBloqueioTaxa();
        ProbAceitacaoTaxa();
        calcTaxaMedia();
    }
}

void Simulate_dAMP() {
    Event *evt = new Event;
    setReqEvent(evt,0);
    long double Max = MinimasDistancias[0][0], OSNRout;
    int orN, deN;
    for (int i = 0; i < Def::getNnodes(); i++) {
        for (int j = 0; j < Def::getNnodes(); j++) {
            if (Max < MinimasDistancias[i][j]) {
                Max = MinimasDistancias[i][j];
                orN = i;
                deN = j;
            }
        }
    } //Encontra a maior entre as menores distancias
    OSNRout = AvaliarOSNR( AllRoutes[orN*Def::getNnodes() + deN].at(0) , 1 );
    cout << "OSNRin = " << Def::get_OSRNin() << "dB, dAmp = " << Def::get_DistaA() << "km, OSNR = " << OSNRout << "dB" << endl; //primeira rota
    if ( OSNRout < Def::getlimiarOSNR(evt->Esquema,Def::PossiveisTaxas[Def::get_numPossiveisTaxas() - 1]) ) {
        ResultDAmpMenorQueLimiar << Def::get_DistaA() << "\t" << Def::get_OSRNin() << endl;
    } else {
        ResultDAmpMaiorQueLimiar << Def::get_DistaA() << "\t" << Def::get_OSRNin() << endl;
    }
}

int SlotsReq(int Ran, Event *evt) {
    return ceil(Def::PossiveisTaxas[Ran]/(2*log2(evt->Esquema)*Def::get_Bslot()));
}

int sumOccupation(int s) {
    int soma=0;
    for(int origem = 0; origem < Def::getNnodes(); origem++)
        for(int destino = 0; destino < Def::getNnodes(); destino++)
            if( (Topology[origem][destino] > 0.0) && (Topology_S[s][origem][destino] == true) )
                //Se houver enlace entre origem e destino e o slot 's' estiver livre neste enlace
                soma++;
    return soma;
}

int TaxaReq() {
    return floor( General::uniforme(0.0,Def::get_numPossiveisTaxas()) );
}

void TryToConnect(const Route* route, const int NslotsReq, int& NslotsUsed, int& si) {
    //NslotsReq informa a quantidade de slots requisitados para a conexao;
    //si informa o slot inicial da sequencia de slots utilizada;
    si = -1, NslotsUsed = 0; //1 <= NslotsUsed <= Nslots informa quantos slots sao utilizados para a conexao
    switch(Alg_Aloc) {
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
}
