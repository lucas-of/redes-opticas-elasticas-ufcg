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

using namespace std;

//Protótipos de Funções
void AccountForBlocking(int NslotsReq, int NslotsUsed); /*Realiza ações necessárias para quando uma conexão foi bloqueada*/
void AccountForBlockingOSNR(int NslotsReq, int NslotsUsed); /*Realiza ações necessárias para quando uma conexão foi bloqueada*/
long double AvaliarOSNR(const Route *Rota, int NSlotsUsed); /*avalia a ONSR da routa passada como parâmetro*/
bool checkFitSi(const bool* vetDisp, int s, int NslotsReq); /*Indica se a conexao pode ser inserida em [s:s+NslotsReq]*/
bool CheckSlotAvailability(const Route*, const int s); /*Checa se a rota route está disponível para o uso, com o slot s livre em toda a rota*/
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
bool ReleaseSlot(const Route* route, int s); /*Libera o slot s em todos os enlaces da Rota route*/
void RemoveCon(Event*); /*Retira uma conexão da rede - liberando todos os seus slots*/
void RequestCon(Event*); /*Cria uma conexão. Dados dois nós, procura pelo algoritmo de roteamento definido uma rota entre os mesmos. Após encontrar a rota, cria a conexão, e por fim agenda o próximo evento de requisição de conexão.*/
void ScheduleEvent(Event*); /*Programa evento para execução, criando a fila*/
void SDPairReq(int &orN, int &deN); /*cria um par de nó origem e destino, aleatoriamente*/
void setReqEvent(Event*, TIME); /*Cria um evento de requisição a partir do instante de criação (TIME)*/
void Sim(); /*Define parâmetros anteriores à simulação. Escolher aqui como o tráfego é distribuído entre os slots e a heurística que será utilizada*/
void SimCompFFO(); /*Simula testando as diversas heurísticas. Usa tráfego aleatoriamente distribuído. Descomentar linha em main() para usar esse código*/
void Simulate(); /*Função principal. Inicia a simulação, chamando clearMemory(). Então começa a fazer as requisições de acordo com o tipo de Evento que ocorreu, até que a simulação termine.*/
int SlotsReq(); /*gera um número aleatório, sob uma distribuição uniforme, que representará o número de slots que a requisição solicitará.*/
int sumOccupation(int s); /*Encontra a ocupação de um certo slot s em todos os enlaces da rede. Para uso em MostUsed()*/
void TryToConnect(const Route* route, const int NslotsReq, int& NslotsUsed, int& si); /*Tenta alocar na rota route um número NslotsReq de slots. O Algoritmo de Alocação é relevante aqui. Retorna si, o slot inicial (-1 se não conseguiu alocar) e NslotsUsed (número de slots que conseguiu alocar).*/


void Dijkstra(); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede*/
void DijkstraAcum(const int orN, const int deN, const int L); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede, considerando número de maneiras de alocar os slots no enlace como o custo do caminho*/
void DijkstraFormas(const int orN, const int deN, const int L); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede, considerando número de maneiras de alocar os slots no enlace e número de nós até o destino como o custo do caminho*/
void DijkstraSP(); /*Implementa o algoritmo de roteamento de Dijkstra para achar rotas entre quaisquer dois nós da rede*/
void FirstFit(const Route*, const int NslotsReq, int& NslotsUsed, int& si); /*Aloca os slots de acordo com o algoritmo First Fit*/
void FirstFitOpt(const Route*, const int NslotsReq, int& NslotsUsed, int& si); /*Aloca os slots de acordo com o algoritmo First Fit, usando as listas otimizadas*/
void MostUsed(const Route*, const int NslotsReq, int& NslotsUsed, int& si); /*Aloca os slots, procurando dentre os slots que podem atender a requisição aqueles mais utilizados*/
void Random(const Route*, const int NslotsReq, int& NslotsUsed, int& si); /*Dentre os slots que podem atender a solicitação, sorteia um*/


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
    delete []Caminho;
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
        if(Topology_S[s][L_or][L_de] == true)
            return false; //Basta o slot s nao estar disponivel em uma das fibras da rota;
    }
    return true; //O slot s esta disponivel em todas as fibras da rota;
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
        for (int j=0; j < Def::getNnodes() ; j++)
            Topology_S[i][j] = new bool[Def::getNnodes()];
    }

    //Carrega topologia de rede a partir do arquivo Topology.txt
    AllRoutes = new vector<Route*>[Def::getNnodes()*Def::getNnodes()];
    int orN, deN;
    for (orN = 0; orN < Def::getNnodes(); orN++) {
        for(deN = 0; deN < Def::getNnodes(); deN++) {
            if (escTop == PacificBell) {
                Topol2 >> Topology[orN][deN];
            } else if (escTop == NSFNet) {
                Topol>>Topology[orN][deN];
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
            if (escTop == PacificBell) {
                Topol2 >> distancia_temp;
            } else if (escTop == NSFNet) {
                Topol>>distancia_temp;
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

void Dijkstra() {
    int orN, deN, VA, i, j, k=0, path, h, hops;
    long double min;
    vector<Node*> r;
    long double *CustoVertice = new long double[Def::getNnodes()];
    int *Precedente = new int[Def::getNnodes()];
    int *PathRev = new int[Def::getNnodes()];
    bool *Status = new bool[Def::getNnodes()];

    //Busca para todos os pares de no a rota mais curta:
    for(orN = 0; orN < Def::getNnodes(); orN++) {
        for(i = 0; i < Def::getNnodes(); i++) {
            if(i != orN)
                CustoVertice[i] = Def::MAX_DOUBLE;
            else
                CustoVertice[i] = 0.0;
            Precedente[i] = -1;
            Status[i] = 0;
        }
        VA = Def::getNnodes();
        while(VA > 0) {
            min = Def::MAX_DOUBLE;
            for(i = 0; i < Def::getNnodes(); i++)
                if((Status[i] == 0)&&(CustoVertice[i] < min)) {
                    min = CustoVertice[i];
                    k = i;
                }
            Status[k] = 1;
            VA = VA-1;
            for(j = 0; j < Def::getNnodes(); j++)
                if((Status[j] == 0)&&(Topology[k][j] != 0)&&(CustoVertice[k]+Topology[k][j] < CustoVertice[j])) {
                    CustoVertice[j] = CustoVertice[k]+Topology[k][j];
                    Precedente[j] = k;
                }
        }

        for(deN = 0; deN < Def::getNnodes(); deN++) {
            path = orN*Def::getNnodes()+deN;
            AllRoutes[path].clear();
            if(deN != orN) {
                PathRev[0] = deN;
                hops = 0;
                j = deN;
                while(j != orN) {
                    hops = hops+1;
                    PathRev[hops] = Precedente[j];
                    j = Precedente[j];
                }
                r.clear();
                for(h = 0; h <= hops; h++)
                    r.push_back(&Rede.at(PathRev[hops-h]));
                AllRoutes[path].push_back(new Route(r));
            }
        }
    }

    //Imprimir Rotas:
    for(orN = 0; orN < Def::getNnodes(); orN++)
        for(deN = 0; deN < Def::getNnodes(); deN++)
            if(orN != deN) {
                cout << endl << "[orN="<<orN<<"  deN="<<deN<<"]  route = ";
                path = orN*Def::getNnodes()+deN;
                hops = AllRoutes[path].at(0)->getNhops();
                cout << hops << " hops: ";
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

void DijkstraSP() {
    int orN, deN, VA, i, j, k=0, path, h, hops;
    long double min;
    vector<Node*> r;
    long double *CustoVertice = new long double[Def::getNnodes()];
    int *Precedente = new int[Def::getNnodes()];
    int *PathRev = new int[Def::getNnodes()];
    bool *Status = new bool[Def::getNnodes()];

    //Busca para todos os pares de no a rota mais curta:
    for(orN = 0; orN < Def::getNnodes(); orN++) {
        for(i = 0; i < Def::getNnodes(); i++) {
            if(i != orN)
                CustoVertice[i] = Def::MAX_DOUBLE;
            else
                CustoVertice[i] = 0.0;
            Precedente[i] = -1;
            Status[i] = 0;
        }
        VA = Def::getNnodes();
        while(VA > 0) {
            min = Def::MAX_DOUBLE;
            for(i = 0; i < Def::getNnodes(); i++)
                if((Status[i] == 0)&&(CustoVertice[i] < min)) {
                    min = CustoVertice[i];
                    k = i;
                }
            Status[k] = 1;
            VA = VA-1;
            for(j = 0; j < Def::getNnodes(); j++)
                if((Status[j] == 0)&&(Topology[k][j] != 0)&&(CustoVertice[k]+Caminho[j].at(k).get_comprimento() < CustoVertice[j])) {
                    CustoVertice[j] = CustoVertice[k]+Caminho[j].at(k).get_comprimento();
                    Precedente[j] = k;
                }
        }

        for(deN = 0; deN < Def::getNnodes(); deN++) {
            path = orN*Def::getNnodes()+deN;
            AllRoutes[path].clear();
            if(deN != orN) {
                PathRev[0] = deN;
                hops = 0;
                j = deN;
                while(j != orN) {
                    hops = hops+1;
                    PathRev[hops] = Precedente[j];
                    j = Precedente[j];
                }
                r.clear();
                for(h = 0; h <= hops; h++)
                    r.push_back(&Rede.at(PathRev[hops-h]));
                AllRoutes[path].push_back(new Route(r));
            }
        }
    }

    //Imprimir Rotas:
    for(orN = 0; orN < Def::getNnodes(); orN++)
        for(deN = 0; deN < Def::getNnodes(); deN++)
            if(orN != deN) {
                cout << endl << "[orN="<<orN<<"  deN="<<deN<<"]  route = ";
                path = orN*Def::getNnodes()+deN;
                hops = AllRoutes[path].at(0)->getNhops();
                cout << hops << " hops: ";
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

void DijkstraAcum(const int orN, const int deN, const int L) {
    //L e a largura de banda (em numero de slots) da requisicao
    assert(deN != orN);
    int VA, i, j, s, k=0, h, path, hops, hopsAux;
    long double min, custoAux;
    vector<Node*> r;
    long double *CustoVertice = new long double[Def::getNnodes()];
    bool **DispVertice = new bool*[Def::getNnodes()];
    for (int i=0 ; i<Def::getNnodes() ; i++) DispVertice[i] = new bool[Def::getSE()];
    int *HopsVertice = new int[Def::getNnodes()]; //O numero de hops ate chegar aquele no pelo caminho mais curto
    bool *DispAux = new bool[Def::getSE()];
    int *Precedente = new int[Def::getNnodes()]; //Informa em cada no por onde chegou a rota mais curta
    int *PathRev = new int[Def::getNnodes()];
    bool *Status = new bool[Def::getNnodes()]; //Informa os nos marcados e nao marcados
    //Busca a rota mais curta para os nos orN e deN;
    //Inicializacao do algoritmo:
    for(i = 0; i < Def::getNnodes(); i++) {
        if(i != orN) {
            CustoVertice[i] = Def::MAX_DOUBLE;
            HopsVertice[i] = Def::MAX_INT;
            for(s = 0; s < Def::getSE(); s++)
                DispVertice[i][s] = false; //O slot e assumido indisponivel
        } else {
            CustoVertice[i] = 0.0;
            HopsVertice[i] = 0;
            for(s = 0; s < Def::getSE(); s++)
                DispVertice[i][s] = true; //O slot e assumido disponivel
        }
        Precedente[i] = -1;
        Status[i] = 0; //No ainda nao foi marcado
    }
    //Processo de Dijkstra:
    VA = Def::getNnodes();
    while(VA > 0) {
        //Toma o no de menor custo ainda nao marcado
        min = Def::MAX_DOUBLE;
        for(i = 0; i < Def::getNnodes(); i++)
            if((Status[i] == false)&&(CustoVertice[i] < min)) {
                min = CustoVertice[i];
                k = i; //No k e o de menor custo
            }
        Status[k] = true; //No k passa a ser marcado
        //A partir do no k, atualiza ou nao o custo de seus nos vizinhos (j)
        VA = VA-1;
        for(j = 0; j < Def::getNnodes(); j++)
            if((Status[j] == false)&&(Topology[k][j] != 0)) {
                //O no j e nao marcado e vizinho do no k
                //Calcula O vetor de disponibilidade em j por uma rota proveniente de k
                for(s = 0; s < Def::getSE(); s++)
                    DispAux[s] = DispVertice[k][s] * !Topology_S[s][k][j];
                //Calcula o numero de hops em j por uma rota proveniente de k
                hopsAux = HopsVertice[k]+1;
                //Checa se o custo em j deve ser atualizado
                custoAux = Heuristics::calculateCostRouting(hopsAux, DispAux, L);
                if(custoAux < CustoVertice[j]) {
                    HopsVertice[j] = hopsAux;
                    CustoVertice[j] = custoAux;
                    for(int s = 0; s < Def::getSE(); s++)
                        DispVertice[j][s] = DispAux[s];
                    Precedente[j] = k;
                }
            }
    }
    //Descrever a rota selecionada:
    PathRev[0] = deN;
    hops = 0;
    j = deN;
    while(j != orN) {
        hops = hops+1;
        PathRev[hops] = Precedente[j];
        j = Precedente[j];
    }
    r.clear();
    for(h = 0; h <= hops; h++)
        r.push_back(&Rede.at(PathRev[hops-h]));
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

void DijkstraFormas(const int orN, const int deN, const int L) {
    //L e a largura de banda (em numero de slots) da requisicao
    assert(orN != deN);
    int VA, i, j, k=0, path, h, hops;
    long double min;
    bool *DispLink = new bool[Def::getSE()];
    long double *CustoVertice = new long double[Def::getNnodes()];
    long double custoLink;
    int *Precedente = new int[Def::getNnodes()];
    int *PathRev = new int[Def::getNnodes()];
    bool *Status = new bool[Def::getNnodes()];
    //Busca para todos os pares de no a rota mais curta:
    for(i = 0; i < Def::getNnodes(); i++) {
        if(i != orN)
            CustoVertice[i] = Def::MAX_DOUBLE;
        else
            CustoVertice[i] = 0.0;
        Precedente[i] = -1;
        Status[i] = 0;
    }
    VA = Def::getNnodes();
    while(VA > 0) {
        //Procura o vertice de menor custo
        min = Def::MAX_DOUBLE;
        for(i = 0; i < Def::getNnodes(); i++)
            if((Status[i] == 0)&&(CustoVertice[i] < min)) {
                min = CustoVertice[i];
                k = i;
            }
        Status[k] = 1; //k e o vertice de menor custo;
        VA = VA-1;
        //Verifica se precisa atualizar ou nao os vizinhos de k
        for(j = 0; j < Def::getNnodes(); j++)
            if((Status[j] == 0)&&(Topology[k][j] != 0)) {
                //O no j e nao marcado e vizinho do no k
                //Calcula O vetor de disponibilidade do enlace entre k e j
                for(int s = 0; s < Def::getSE(); s++)
                    DispLink[s] = !Topology_S[s][k][j];
                custoLink = Heuristics::calculateCostLink(DispLink, L);
                if(CustoVertice[k] + custoLink < CustoVertice[j]) {
                    CustoVertice[j] = CustoVertice[k] + custoLink;
                    Precedente[j] = k;
                }
            }
    }

    //Formar a rota:
    path = orN*Def::getNnodes()+deN;
    AllRoutes[path].clear();
    PathRev[0] = deN;
    hops = 0;
    j = deN;
    while(j != orN) {
        hops = hops+1;
        PathRev[hops] = Precedente[j];
        j = Precedente[j];
    }
    vector<Node*> r;
    r.clear();
    for(h = 0; h <= hops; h++)
        r.push_back(&Rede.at(PathRev[hops-h]));
    assert(r.at(0)->get_whoami() == orN && r.at(hops)->get_whoami() == deN);
    AllRoutes[path].push_back(new Route(r));

    delete []CustoVertice;
    delete []Precedente;
    delete []Status;
    delete []PathRev;
    delete []DispLink;
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

void FirstFit(const Route* route, const int NslotsReq, int& NslotsUsed, int& si) {
    si = -1;
    NslotsUsed = 0; //Valores nao permitidos
    int sum;
    for(int s = 0; s <= Def::getSE() - NslotsReq; s++) {
        sum = 0;
        for(int se = s; se < s + NslotsReq; se++) {
            if(CheckSlotAvailability(route, se))
                sum++;
            else
                break;
        }
        if(sum == NslotsReq) { //conseguiu alocar slots
            si = s;
            NslotsUsed = NslotsReq;
            break;
        }
    }
}

void FirstFitOpt(const Route* route, const int NslotsReq, int& NslotsUsed, int& si) {
    assert( (si == -1) && (NslotsUsed == 0) );
    int s, sum=0;
    for(int sOrd = 0; sOrd < Def::getSE(); sOrd++) {
        s = FFlists[NslotsReq]->at(sOrd);
        if(s <= Def::getSE()-NslotsReq) {
            //si e capaz de suportar a requisicao;
            sum = 0;
            for(int se = s; se < s + NslotsReq; se++)
                if(CheckSlotAvailability(route, se))
                    sum++;
                else
                    break;
        }
        if(sum == NslotsReq) { //O slot s pode atender a requisicao
            si = s;
            NslotsUsed = NslotsReq;
            break;
        }
    }
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
    int Npontos, aux;
    long double op;

    cout << "Usar a topologia Pacific Bell <" << PacificBell << "> ou NSFNet <" << NSFNet << ">?" << endl;
    cin>>aux;
    escTop = (Topologia)aux;

    //Adquire o numero de Nos:
    if (escTop == PacificBell) {
        Topol2 >> aux;
    } else if (escTop == NSFNet) {
        Topol>>aux;
    }
    Def::setNnodes(aux);
    cout << "Numero de nos: "<< Def::getNnodes() << endl;

    cout<<"Considera a OSNR? <"<<SIM<<"> Sim ou <"<<NAO<<"> Nao"<<endl;
    cin>>aux;
    AvaliaOsnr = (Respostas)aux;
    cout << AvaliaOsnr << endl;

    if (AvaliaOsnr == SIM) {
        cout << "Entre com a banda de um slot, em GHz (valores comuns sao 1.5625, 3.125, 6.25 e 12.5)" << endl;
        cin >> op;
        Def::setBslot(op);

        cout << "Entre com a relação entre taxa de transf. e num. de slots. 1:1 - "<< Def::COMP1 << " 2:1 - " << Def::COMP2 << " 4:1 - "<< Def::COMP4 << endl;
        cin >> op;
        Def::setCompressao((Def::Compressao) op);
    }

    if (escTop == PacificBell) {
        Topol2 >> aux;
    } else if (escTop == NSFNet) {
        Topol>>aux;
    }
    Def::setSE(aux); //o enlace tem 100GHz de banda
    cout << "Numero de Slots por Enlace: " << Def::getSE() << endl;

    //Outras entradas para o simulador
    cout<<"Entre com a Quantidade maxima de Slots por Requisicao [1 <= SR <= "<<Def::getSE()<<"]: ";
    cin >> aux;
    Def::setSR(aux); //Uma requisicao nao podera pedir mais que aux slots

    cout << DJK<<" - DJK \n"<<DJK_Formas<<" - DJK_Formas \n"<< DJK_Acum<<" - DijkstraAcumulado "<< endl << SP << " - Shortest Path"<<endl;
    cout << "Entre com o Algoritmo de Roteamento: ";
    cin >> Alg_Routing;

    cout<<RD<<" - Random \n"<<FF<<" - FirstFit \n"<<MU<<" - Most Used \n"<<FFO<<" - FirstFitOpt "<<endl;
    cout << "Entre com o Algoritmo de Alocacao: ";
    cin >> Alg_Aloc;

    //Cria as listas First Fit
    if(Alg_Aloc == FFO || Alg_Aloc == FF) {
        FFlists = new vector<int>*[Def::getSR()+1];
        for(int i = 0; i < Def::getSR()+1; i++)
            FFlists[i] = new vector<int>(0);
    }

    if (AvaliaOsnr==SIM) {
        double OSNR;
        cout << "Entre com o limiar de OSNR para o qual a conexao e estabelecida: " << endl;
        cin >> OSNR;
        Def::setLimiarOSNR(OSNR);
    }

    cout <<"Entre com o mu (taxa de desativacao de conexoes): ";
    cin >> mu; //mu = taxa de desativacao das conexoes;
    cout << "La = Taxa de Chegada de Conexoes. Entre com..." << endl;
    cout << "LaNet minimo = ";
    cin >> LaNetMin; // La = taxa de chegada das conexoes;
    cout <<"LaNet maximo = ";
    cin >> LaNetMax;
    cout<<"#Pontos no grafico = ";
    cin >> Npontos;
    LaPasso = (LaNetMax-LaNetMin)/(Npontos-1);

    if (AvaliaOsnr==SIM) {
        cout << "Entre com a potencia de entrada, em dBm." << endl;
        cin>>op;
        Def::set_Pin(op);
        cout<<"Entre com distancia entre os amplificadores"<<endl;
        cin>>op;
        Def::set_DistaA(op);
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
    cout<<"Entre com o numero maximo de requisicoes que o programa vai rodar antes de terminar: ";
    cin>> nR;
    Def::setNumReqMax(nR);

    for(int f = 0; f <= Def::getSE()-Def::getSR()+1; f++)
        Metrica<<f<<"\t"<<1.0/(f+1)<<endl;
}

void MostUsed(const Route* route, const int NslotsReq, int& NslotsUsed, int& si) {
    int *vetSlotsUsed = new int[Def::getSE()];
    bool *vetDisp = new bool[Def::getSE()];

    //Checa a disponibilidade no caminho 'path' para cada slot s;
    for(int s = 0; s < Def::getSE(); s++)
        vetDisp[s] = CheckSlotAvailability(route, s);

    //Carrega vetSlotsUsed com o numero de enlaces ocupados em cada slot;
    int soma;
    for(int s = 0; s < Def::getSE(); s++) {
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
    for(int s = 0; s <= Def::getSE()-NslotsReq; s++) {
        fit = true;
        soma = 0;
        for(int b = s; b < s + NslotsReq; b++)
            if(vetDisp[b] == true) {
                assert(vetSlotsUsed[b] >= 0);
                soma += vetSlotsUsed[b];
            } else {
                fit = false;
                break;
            }
        if(fit == true && soma > maxSoma) { //A requisicao se encaixa entre os slots s,s+1,...,s+NslotsReq-1 e nesta posicao havera a maior soma de enlaces ocupados na rede;
            maxSoma = soma;
            NslotsUsed = NslotsReq;
            si = s;
        }
    }
    delete []vetSlotsUsed;
    delete []vetDisp;
}

void Random(const Route* route, const int NslotsReq, int& NslotsUsed, int& si) {
    int soma=0, somaAlocacao=0, alocarSlot;
    bool *vetDisp = new bool[Def::getSE()];
    int *vetAloc = new int[Def::getSE()];
    for(int s = 0; s < Def::getSE(); s++)
        vetAloc[s] = 0;

    //Checa a disponibilidade no caminho 'path' para cada slot s;
    for(int s = 0; s < Def::getSE(); s++)
        vetDisp[s] = CheckSlotAvailability(route, s);
    //Carrega vetSlotsUsed com o numero de enlaces ocupados em cada slot;
    bool fit;
    NslotsUsed = 0;
    si = -1;
    for(int s = 0; s <= Def::getSE() - NslotsReq; s++) {
        fit = true;
        soma = 0;
        for(int b = s; b < s + NslotsReq; b++)
            if(vetDisp[b] == true)
                soma++;
            else {
                fit = false;
                break;
            }
        if(fit == true && soma == NslotsReq) {
            //A requisicao se encaixa entre os slots s,s+1,...,s+NslotsReq-1
            somaAlocacao++; // verifica quantas posicoes estao disponiveis para alocacao
            vetAloc[s] = 1; //posso alocar a partir desta posicao
        }
    }

    // fazer o sorteio do slot que sera alocada
    if(somaAlocacao > 0) {
        // se existir pelo menos um conjunto slot disponivel com o tamanho da solicitacao
        alocarSlot = rand()% somaAlocacao; // fazer alocacao no conjunto de slot dispovivel "alocarSlot"
        int s = 0;
        while ((s < Def::getSE()) && (alocarSlot >= 0)) {
            if(vetAloc[s] == 1)
                alocarSlot--;
            s++;
        }
        NslotsUsed = NslotsReq;
        si = s-1; // -1 porque foi incrementado no final do while
    } else {
        NslotsUsed = 0;
        si = -1;
    }
    delete []vetDisp;
    delete []vetAloc;
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
    int orN, deN, NslotsReq, NslotsUsed, si;
    SDPairReq(orN, deN);
    //deN = (orN + Def::getNnodes()/2)%Def::getNnodes(); //Nos antipodas no anel
    NslotsReq = SlotsReq();
    Def::numReq++;
    Def::numSlots_Req += NslotsReq;

    if(Alg_Routing == DJK_Formas)
        DijkstraFormas(orN, deN, NslotsReq);
    if(Alg_Routing == DJK_Acum)
        DijkstraAcum(orN, deN, NslotsReq);

    //Para o conjunto de rotas fornecida pelo roteamento, tenta alocar a requisicao:
    Route *route;
    long double OSNR;
    for(unsigned int i = 0; i < AllRoutes[orN*Def::getNnodes()+deN].size(); i++) {
        route = AllRoutes[orN*Def::getNnodes()+deN].at(i); //Tenta a i-esima rota destinada para o par orN-deN
        NslotsUsed = 0;
        si = -1;
        TryToConnect(route, NslotsReq, NslotsUsed, si);
        assert( (NslotsUsed == 0) || (NslotsUsed == NslotsReq) ); //Tirar isso aqui quando uma conexao puder ser atendida com um numero menor de slots que o requisitado
        if(NslotsUsed > 0) { //A conexao foi aceita
            assert(NslotsUsed <= NslotsReq && si >= 0 && si <= Def::getSE()-NslotsUsed);
            if (AvaliaOsnr==SIM) OSNR = AvaliarOSNR(route,NslotsUsed);
            if (AvaliaOsnr==NAO || OSNR >= Def::getlimiarOSNR()) { //aceita a conexao
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
                Conexao *newConexao = new Conexao(route, si, si + NslotsUsed - 1, simTime + General::exponential(mu));
                //Agendar um dos eventos possiveis para conexao (Expandir, contrair, cair, etc):
                Event *evt = new Event;
                evt->conexao = newConexao;
                DefineNextEventOfCon(evt);
                ScheduleEvent(evt);
                break;
            } else { //conexao bloqueada por OSNR
                NslotsUsed = 0;
                AccountForBlockingOSNR(NslotsReq,NslotsUsed);
            }
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
    cout << "nu0= " << laNet << "   PbReq= " << (long double) Def::numReq_Bloq/Def::numReq << "   PbSlots= " << (long double) Def::numSlots_Bloq/Def::numSlots_Req << " HopsMed= " << (long double) Def::numHopsPerRoute/(Def::numReq-Def::numReq_Bloq) << " netOcc= " << (long double) Def::netOccupancy << endl;
    Resul << laNet << "\t" << (long double) Def::numReq_Bloq/Def::numReq << "\t" << (long double) Def::numSlots_Bloq/Def::numSlots_Req << "\t" << (long double) Def::numHopsPerRoute/(Def::numReq-Def::numReq_Bloq) << "\t" << Def::netOccupancy << endl;
    ResulOSNR << laNet << "\t" << Def::numReq_BloqPorOSNR/Def::numReq_Bloq << endl;
}

int SlotsReq() {
    double sum=0.0, x;
    int Lr;
    for(Lr = 1; Lr <= Def::getSR(); Lr++)
        sum += Def::getLaNet(Lr);
    x = General::uniforme(0.0,sum);
    sum = 0.0;
    for(Lr = 1; Lr <= Def::getSR(); Lr++) {
        sum += Def::getLaNet(Lr);
        if(x < sum)
            break;
    }
    Lr = ceil(1.0*Lr/Def::get_Compressao()); /*compressao devido ao esquema de modulação*/
    assert(Lr > 0 && Lr <= Def::getSR());
    return Lr;
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
