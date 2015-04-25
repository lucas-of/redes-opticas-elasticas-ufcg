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
#include <QThread>
#include "Main_Auxiliar.h"
#include "RWA.h"
#include "PSR.h"
#include "Conexao.h"
#include "Def.h"
#include "General.h"
#include "Heuristics.h"
#include "Route.h"
#include "Node.h"
#include "Enlace.h"
#include "ASE_Noise.cpp"
#include "Metricas.cpp"

using namespace std;

//Protótipos de Funções
void AccountForBlocking(int NslotsReq, int NslotsUsed, int nTaxa, Def *Config); /*Realiza ações necessárias para quando uma conexão foi bloqueada*/
void AccountForBlockingOSNR(int NslotsReq, int NslotsUsed); /*Realiza ações necessárias para quando uma conexão foi bloqueada*/
long double AvaliarOSNR(const Route *Rota, int NSlotsUsed); /*avalia a ONSR da routa passada como parâmetro*/
bool checkFitSi(const bool* vetDisp, int s, int NslotsReq); /*Indica se a conexao pode ser inserida em [s:s+NslotsReq]*/
void clearMemory(Def *Config); /*Limpa e zera todas as constantes de Def.h, reinicia o tempo de simulação e libera todos os slots.*/
void CarregaCoeficientes();
void CompressCon(Event*, Def *Config); /*Diminui a quantidade de slots reservados para a conexão. Pode ser configurada para retirar sempre o slot da direita ou o da esquerda ou o da direita, escolhido aleatoriamente. Lembrar de conferir isto.*/
void CompressRandom(Conexao *con, Def *Config); /*Comprime conexão, removendo slot da esquerda ou da direita*/
void CompressRight(Conexao *con, Def *Config); /*Comprime conexão, removendo o slot da direita*/
void createStructures(); /*Cria estrutura topológica da rede, e carrega os dados de Topology.txt*/
void DefineNextEventOfCon(Event* evt); /*Define se o próximo evento da conexão será uma expansão, compressão ou desativação da conexão*/
void ExpandCon(Event*, Def *Config); /*Exprime conexão, inserindo um novo slot disponível para a mesma*/
void EncontraMultiplicador(Def *Config); /*calcula a OSNR para o diâmetro da rede, após multiplicar a distância dos enlaces por certo fator*/
bool FillSlot(const Route* route, const int s, const bool b, Def *Config); /*Preenche todos os slots s da rota route com o valor b (ou seja, ocupa ou livra o slot s de todos os enlaces da rota)*/
void GrauDosNodes(Def *Config); /*Calcula o grau dos nós*/
void Load(); /*Função que lê os dados relativos à simulação. Realiza tarefas de io. Verificar significado de várias variáveis em seu escopo*/
void RefreshNoise(Def *Config); /*atualiza os ruídos dos enlaces*/
bool ReleaseSlot(const Route* route, int s, Def *Config); /*Libera o slot s em todos os enlaces da Rota route*/
void RemoveCon(Event*, Def *Config); /*Retira uma conexão da rede - liberando todos os seus slots*/
void RequestCon(Event*, Def *Config); /*Cria uma conexão. Dados dois nós, procura pelo algoritmo de roteamento definido uma rota entre os mesmos. Após encontrar a rota, cria a conexão, e por fim agenda o próximo evento de requisição de conexão.*/
void ScheduleEvent(Event*); /*Programa evento para execução, criando a fila*/
void SDPairReq(int &orN, int &deN); /*cria um par de nó origem e destino, aleatoriamente*/
void setReqEvent(Event*, TIME); /*Cria um evento de requisição a partir do instante de criação (TIME)*/
long double Simula_Rede(Def *Config);
void SimPbReq(); /*Simulação para Probabilidade de Bloqueio*/
void SimOSNR(); /*Simulação para OSNR*/
void SimNSlots(Def *Config);
void SimAlfaBeta();
void Sim(); /*Define parâmetros anteriores à simulação. Escolher aqui como o tráfego é distribuído entre os slots e a heurística que será utilizada*/
void SimCompFFO(); /*Simula testando as diversas heurísticas. Usa tráfego aleatoriamente distribuído. Descomentar linha em main() para usar esse código*/
void Simulate(Def *Config); /*Função principal. Inicia a simulação, chamando clearMemory(). Então começa a fazer as requisições de acordo com o tipo de Evento que ocorreu, até que a simulação termine.*/
void Simulate_dAMP(Def *Config); /*Análogo de Simulate(), mas para variações na distância entre os amplificadores*/
int SlotsReq(int Ran, Event *evt); /*coverte a taxa em um número de slots.*/
int TaxaReq();  /*gera um número aleatório, sob uma distribuição uniforme, que representará a taxa de transmissão que a requisição solicitará.*/
void TryToConnect(const Route* route, const int NslotsReq, int& NslotsUsed, int& si, Def *Config); /*Tenta alocar na rota route um número NslotsReq de slots. O Algoritmo de Alocação é relevante aqui. Retorna si, o slot inicial (-1 se não conseguiu alocar) e NslotsUsed (número de slots que conseguiu alocar).*/

int main() {
	Load();
	cout << "Inicio da simulacao:" << endl;
	createStructures();
	if (MAux::Alg_Routing == MH)
		RWA::Dijkstra();
	else if (MAux::Alg_Routing == SP)
		RWA::DijkstraSP();
	else if(MAux::Alg_Routing == OSNRR)
		RWA::OSNRR();

	if (MAux::escSim == Sim_PbReq)
		SimPbReq();
	else if (MAux::escSim == Sim_OSNR)
		SimOSNR();
	else if (MAux::escSim == Sim_DAmp) {
		Simulate_dAMP(MAux::Config);
		//EncontraMultiplicador();
	} else if (MAux::escSim == Sim_NSlots)
		SimNSlots(MAux::Config);
	else if (MAux::escSim == Sim_TreinoPSR) {
		PSR(4);
		PSR::executar_PSR();
	} else if (MAux::escSim == Sim_AlfaBetaOtimizado) {
		SimAlfaBeta();
	}

	delete []MAux::Topology;
	delete []MAux::AllRoutes;
	delete []MAux::Caminho;
	delete []MAux::Coeficientes;
	delete []MAux::MinimasDistancias;
	if (MAux::FFlists != NULL)
		for (int i = 0; i < Def::getSR()+1; i++)
			delete MAux::FFlists[i];
	delete[] MAux::FFlists;
	cout << "Fim do programa";
	cin.get();
	cin.get();
	return 0;
}

void AccountForBlocking(int NslotsReq, int NslotsUsed, int nTaxa, Def *Config) {
	if(NslotsUsed <= 0) { //A conexao foi bloqueada
		Config->numReq_Bloq++;
		Config->numReqBloq_Taxa[nTaxa]++;
	}
	Config->numSlots_Bloq += (NslotsReq - NslotsUsed);
}

void CarregaCoeficientes() {
	int N;
	PSR::PSO_Coeficientes_R >> N;
	PSR PSR_Auxiliar(N);

	MAux::Coeficientes = new long double[N*N];
		for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			PSR::PSO_Coeficientes_R >> MAux::Coeficientes[i*N + j];
		}
	}

	for (int i = 0; i < Def::getNnodes(); i++) {
		for (int j = 0; j < Def::getNnodes(); j++) {
			MAux::Caminho[i].at(j).recalcular_peso(MAux::Coeficientes);
		}
	}
}

bool checkFitSi(const bool* vetDisp, int s, int NslotsReq) {
	assert(s + NslotsReq - 1 < Def::getSE());
	for(int b = s; b < s + NslotsReq; b++)
		if(vetDisp[b] == false)
			return false;
	return true;
}

void clearMemory(Def *Config) {
	int s, L_or, L_de;
	Conexao *con;
	Event *evtPtr;
	const Route* route;
	Config->numReq = Config->numReq_Bloq = Config->numSlots_Req = Config->numSlots_Bloq = Config->numHopsPerRoute = Config->netOccupancy = MAux::simTime = Config->numReq_BloqPorOSNR = Config->numSlots_BloqPorOSNR = Config->taxaTotal = 0.0;
	while(MAux::firstEvent != NULL) {
		if(MAux::firstEvent->conexao != NULL) {
			//Há uma conexao
			con = MAux::firstEvent->conexao;
			route = con->getRoute();
			for (unsigned c = 0; c < route->getNhops(); c++) {
				//remove todos os slots ocupados da conexao
				L_or = route->getNode(c);
				L_de = route->getNode(c+1);
				for (s = con->getFirstSlot(); s <= con->getLastSlot(); s++)
					Config->Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] = false;
			}
			delete con;
		}
		evtPtr = MAux::firstEvent;
		MAux::firstEvent = MAux::firstEvent->nextEvent;
		delete evtPtr;
	}

	for (int i = 0; i<Def::get_numPossiveisTaxas(); i++) {
		Config->numReqBloq_Taxa[i] = 0;
		Config->numReq_Taxa[i] = 0;
		Config->tempoTotal_Taxa[i] = 0;
	}

	for (int i = 0; i<Def::numEsquemasDeModulacao; i++) {
		Config->numReqAceit_Esquema[i] = 0;
		Config->taxaTotal_Esquema[i] = 0;
	}

	//Checar se limpeza foi realizada corretamente
	for(int orN = 0; orN < Def::getNnodes(); orN++)
		for(int deN = 0; deN < Def::getNnodes(); deN++)
			for(s = 0; s < Def::getSE(); s++)
                assert(Config->Topology_S[s*Def::Nnodes*Def::Nnodes + orN*Def::Nnodes + deN] == false);
	assert(MAux::firstEvent == NULL);
}

void CompressCon(Event* evt, Def *Config) {
	if (MAux::ExpComp) {
		//Remover um slot lateral
		Conexao *con = evt->conexao;
		CompressRight(con, Config); //Remove sempre o slot da direita. Tentar outras heuristicas
		//CompressRandom(con);
		assert(con->getLastSlot() >= con->getFirstSlot()); //Apenas para checar erros
		DefineNextEventOfCon(evt);
		ScheduleEvent(evt);
	}
}

void CompressRandom(Conexao *con, Def *Config)  {
	//Remove aleatoriamente o slot da direita ou da esquerda.
	if(rand() % 2) {
		//Comprime a esquerda
		FillSlot(con->getRoute(), con->getFirstSlot(), false, Config);
		con->incFirstSlot(+1);
	} else {
		//Comprime a direita
		FillSlot(con->getRoute(), con->getLastSlot(), false, Config);
		con->incLastSlot(-1);
	}
}

void CompressRight(Conexao *con, Def *Config) {
	//Remove sempre o slot da direita.
	FillSlot(con->getRoute(), con->getLastSlot(), false, Config);
	con->incLastSlot(-1);
}

void createStructures() {
	MAux::Topology = new long double[Def::Nnodes*Def::Nnodes]; //matriz de conexões entre nós

	//Carrega topologia de rede a partir do arquivo Topology.txt
	MAux::AllRoutes = new vector<Route*>[Def::getNnodes()*Def::getNnodes()];
	int orN, deN;
	for (orN = 0; orN < Def::getNnodes(); orN++) {
		for(deN = 0; deN < Def::getNnodes(); deN++) {
			switch(MAux::escTop) {
				case PacificBell: MAux::Topol2 >> MAux::Topology[orN*Def::Nnodes+deN]; break;
				case NSFNet: MAux::Topol>>MAux::Topology[orN*Def::Nnodes+deN]; break;
				case NFSNetMod: MAux::Topol5>>MAux::Topology[orN*Def::Nnodes+deN]; break;
				case PontoaPonto8: MAux::Topol3>>MAux::Topology[orN*Def::Nnodes+deN]; break;
				case PontoaPonto4: MAux::Topol4>>MAux::Topology[orN*Def::Nnodes+deN]; break;
				case Top1: MAux::Topol6>>MAux::Topology[orN*Def::Nnodes+deN]; break;
				case Top2: MAux::Topol7>>MAux::Topology[orN*Def::Nnodes+deN]; break;
			}
			cout<<MAux::Topology[orN*Def::Nnodes+deN]<<" ";
		}
		cout << endl;
	}

	//Calcula o grau de cada no
	GrauDosNodes(MAux::Config);

	for (int i=0;i < Def::getNnodes() ; i++) {
		MAux::Rede.push_back(Node(i));
	}

	double multiplicador;
	switch(MAux::escTop) {
		case PacificBell: MAux::Topol2 >> multiplicador; break;
		case NSFNet: MAux::Topol>>multiplicador; break;
		case NFSNetMod: MAux::Topol5>>multiplicador; break;
		case PontoaPonto8: MAux::Topol3>>multiplicador; break;
		case PontoaPonto4: MAux::Topol4>>multiplicador; break;
		case Top1: MAux::Topol6>>multiplicador; break;
		case Top2: MAux::Topol7>>multiplicador; break;
	}

	//Implemente os Enlaces
	MAux::Caminho = new vector<Enlace>[Def::getNnodes()];
	for (int i=0; i < Def::getNnodes(); i++){
		for(int j=0; j < Def::getNnodes(); j++){
			double distancia_temp;
			switch(MAux::escTop) {
				case PacificBell: MAux::Topol2 >> distancia_temp; break;
				case NSFNet: MAux::Topol>>distancia_temp; break;
				case NFSNetMod: MAux::Topol5>>distancia_temp; break;
				case PontoaPonto8: MAux::Topol3>>distancia_temp; break;
				case PontoaPonto4: MAux::Topol4>>distancia_temp; break;
				case Top1: MAux::Topol6>>distancia_temp; break;
				case Top2: MAux::Topol7>>distancia_temp; break;
			}
			if(MAux::Topology[i*Def::Nnodes + j] == 1){
				MAux::Caminho[i].push_back(Enlace(&MAux::Rede.at(i),&MAux::Rede.at(j),multiplicador*distancia_temp));
			} else {
				MAux::Caminho[i].push_back(Enlace(NULL,NULL,Def::MAX_INT));
			}
		}
	}

	if (MAux::Alg_Routing == PSO) CarregaCoeficientes();
}

void DefineNextEventOfCon (Event* evt) {
	//Recalcula Te (tempo de expansao) e Tc (tempo de compressao) e os compara com o tempo para desativacao
	//So pode haver expansao se o numero de slots for menor do que Def::getSE()

	Conexao *con = evt->conexao;
	TIME Te, Tc, evtTime;
	EventType evtType = UNKNOWN;
	if ( (MAux::ExpComp == true) && ((con->getLastSlot() - con->getFirstSlot() + 1) < Def::getSE()) )
		//Se o numero de slots utilizados for menor do que Def::getSE()
		Te = MAux::simTime + General::exponential(MAux::laE);
	else
		Te = Def::MAX_DOUBLE;
		//So pode haver compressao se o numero de slots for maior ou igual a 2, pois 1 e o numero minimo do slots que podem estar ativos para uma conexao

	if ( (MAux::ExpComp == true) && (con->getLastSlot() - con->getFirstSlot() > 0) )
		Tc = MAux::simTime + General::exponential(MAux::muC);
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


void ExpandCon(Event* evt, Def *Config) {
	if (MAux::ExpComp) {
		Conexao *con = evt->conexao;
		//Procura um slot lateral. Assume-se que tenta primeiro o slot a esquerda e depois o a direita.
		if(con->getFirstSlot() > 0 && RWA::CheckSlotAvailability(con->getRoute(), con->getFirstSlot()-1, Config)) {
			FillSlot(con->getRoute(), con->getFirstSlot()-1, true, Config);
			con->incFirstSlot(-1);
		} else if(con->getLastSlot()<Def::getSE()-1 && RWA::CheckSlotAvailability(con->getRoute(),con->getLastSlot()+1, Config)) {
			FillSlot(con->getRoute(), con->getLastSlot()+1, true, Config);
			con->incLastSlot(+1);
		} else
			Config->numSlots_Bloq++;
		Config->numSlots_Req++;
		DefineNextEventOfCon(evt);
		ScheduleEvent(evt);
	}
}

bool FillSlot(const Route* route, const int s, const bool b, Def *Config) {
	int L_or, L_de;
	for (unsigned c = 0; c < route->getNhops(); c++) {
		L_or = route->getNode(c);
		L_de = route->getNode(c+1);
		assert(Config->Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] != b);
		Config->Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] = b;
	}
	return true;
}

void GrauDosNodes(Def *Config) {
	int node_temp = 0;
	Config->clearGrauNo();
	for (int orN = 0; orN < Def::getNnodes() ; orN++) {
		for (int deN = 0; deN < Def::getNnodes() ; deN++) {
			if (MAux::Topology[orN*Def::Nnodes+deN] == 1) node_temp++;
		}
		Def::setGrauNo(node_temp);
		node_temp=0;
	}
}

void Load() {
	Def::setPossiveisTaxas();

	int Npontos, aux;
	long double op;

	cout << "Escolha a Simulação. " << endl << "\tProbabilidade de Bloqueio <" << Sim_PbReq << ">;" << endl << "\tOSNR <" << Sim_OSNR << ">; " << endl << "\tDistancia dos Amplificadores <" << Sim_DAmp << ">;" << endl << "\tNumero de Slots <" << Sim_NSlots << ">;" << endl << "\tPSR - Otimização <" << Sim_TreinoPSR << ">;" << endl << "\tOtimização do Alfa/Beta <" << Sim_AlfaBetaOtimizado << ">." << endl;
	cin >> aux;
	MAux::escSim = (Simulacao)aux;

	if (MAux::escSim == Sim_AlfaBetaOtimizado) {
		cout << "Otimizar para AWR com Distancia ou AWR com Ruido?\n\t";
		cout << OtimizarAlfa << " - Distancia\n\t" << OtimizarBeta << " - Ruido\n";
		cin >> aux;
		MAux::escOtim = (SimOtimizacao)aux;
	}

	cout << "Escolha a topologia." << endl << "\tPacific Bell <" << PacificBell << ">; "<< endl << "\tNSFNet <" << NSFNet << ">; " << endl << "\tNSFNet Modificada (Reduzida) <" << NFSNetMod << ">;" << endl << "\tPonto a Ponto de 4 Nós <" << PontoaPonto4 <<">; "  << endl << "\tPonto a Ponto de 8 Nós <" << PontoaPonto8 << ">; " << endl << "\tTop1 <" << Top1 << ">;" << endl << "\tTop2 <" << Top2 << ">;" << endl;
	cin>>aux;
	MAux::escTop = (Topologia)aux;

	//Adquire o numero de Nos:
	switch (MAux::escTop) {
		case PacificBell: MAux::Topol2 >> aux; break;
		case NSFNet: MAux::Topol>>aux; break;
		case NFSNetMod: MAux::Topol5>>aux; break;
		case PontoaPonto4: MAux::Topol4>>aux; break;
		case PontoaPonto8: MAux::Topol3>>aux; break;
		case Top1: MAux::Topol6>>aux; break;
		case Top2: MAux::Topol7>>aux; break;
	}

	Def::setNnodes(aux);
    cout << "Numero de nos: "<< Def::getNnodes() << endl;

	cout<<"Considera a OSNR? <"<<SIM<<"> Sim ou <"<<NAO<<"> Nao"<<endl;
	cin>>aux;
	MAux::AvaliaOsnr = (Respostas)aux;
	cout << MAux::AvaliaOsnr << endl;

	cout << "Entre com a banda de um slot, em GHz (valores comuns sao 1.5625, 3.125, 6.25 e 12.5)" << endl;
	cin >> op;
	Def::setBslot(op);
	Def::setBref(12.5);

	switch (MAux::escTop) {
		case PacificBell: MAux::Topol2 >> aux; break;
		case NSFNet: MAux::Topol>>aux; break;
		case NFSNetMod: MAux::Topol5>>aux; break;
		case PontoaPonto4: MAux::Topol4>>aux; break;
		case PontoaPonto8: MAux::Topol3>>aux; break;
		case Top1: MAux::Topol6>>aux; break;
		case Top2: MAux::Topol7>>aux; break;
	}
	Def::setSE(aux); //o enlace tem 100GHz de banda
	cout << "Numero de Slots por Enlace: " << Def::getSE() << endl;

	//Outras entradas para o simulador
	Def::setSR(Def::getSE()); //Uma requisicao nao podera pedir mais que SE slots

    delete MAux::Config;
    MAux::Config = new Def(); //redefine Config para realocar Topology_S
    
	if ((MAux::escSim != Sim_TreinoPSR) && (MAux::escSim != Sim_AlfaBetaOtimizado)) {
		cout << "\t" << MH<<" - Minimum Hops \n\t"<<CSP<<" - CSP\n\t"<< CSP_Acum<<" - CSP Acumulado\n\t" << SP << " - Shortest Path\n\t"<< DJK_SPeFormas << " - AWR\n\t" << DJK_RuidoEFormas << " - AWR com Ruído do Enlace\n\t" << LOR_Modificado << " - LOR Modificado\n\t" << PSO << " - PSO\n\t" << OSNRR << " - OSNR-R\n";
		cout << "Entre com o Algoritmo de Roteamento: ";
		cin >> MAux::Alg_Routing;
	}
	if (MAux::escSim == Sim_AlfaBetaOtimizado) {
		if (MAux::escOtim == OtimizarAlfa)
			MAux::Alg_Routing = DJK_SPeFormas;
		else if (MAux::escOtim == OtimizarBeta)
			MAux::Alg_Routing = DJK_RuidoEFormas;
	}
	if (MAux::Alg_Routing == OSNRR)
		MAux::AvaliaOsnr = SIM;

	cout<<"\t" << RD<<" - Random \n\t"<<FF<<" - FirstFit \n\t"<<MU<<" - Most Used \n\t"<<FFO<<" - FirstFitOpt "<<endl;
	cout << "Entre com o Algoritmo de Alocacao: ";
	cin >> MAux::Alg_Aloc;

	//Cria as listas First Fit
	if((MAux::Alg_Aloc == FFO) || (MAux::Alg_Aloc == FF)) {
		MAux::FFlists = new vector<int>*[Def::getSR()+1];
		for(int i = 0; i < Def::getSR()+1; i++)
			MAux::FFlists[i] = new vector<int>(0);
	}

	cout <<"Entre com o mu (taxa de desativacao de conexoes): ";
	cin >> MAux::mu; //mu = taxa de desativacao das conexoes;
	if (MAux::escSim == Sim_OSNR || MAux::escSim == Sim_DAmp) {
		cout << "Entre com..." << endl;
		cout << "OSNR minimo = ";
		cin >> MAux::OSNRMin; // La = taxa de chegada das conexoes;
		cout <<"OSNR maximo = ";
		cin >> MAux::OSNRMax;
		cout<<"#Pontos no grafico = ";
		cin >> Npontos;
		MAux::OSNRPasso = (MAux::OSNRMax-MAux::OSNRMin)/(Npontos-1);
	}
	if (MAux::escSim == Sim_PbReq) {
		cout << "La = Taxa de Chegada de Conexoes. Entre com..." << endl;
		cout << "LaNet minimo = ";
		cin >> MAux::LaNetMin; // La = taxa de chegada das conexoes;
		cout <<"LaNet maximo = ";
		cin >> MAux::LaNetMax;
		cout<<"#Pontos no grafico = ";
		cin >> Npontos;
		MAux::LaPasso = (MAux::LaNetMax-MAux::LaNetMin)/(Npontos-1);
	}
	if (MAux::escSim == Sim_TreinoPSR || MAux::escSim == Sim_AlfaBetaOtimizado) {
		cout << "La = Taxa de Chegada de Conexoes. Entre com..." << endl;
		cout << "LaNet = ";
		cin >> MAux::laNet; // La = taxa de chegada das conexoes;
	}
	if (MAux::escSim == Sim_DAmp) {
		cout << "Entre com..." << endl;
		cout << "Distancia minima entre Amplf. de Linha = ";
		cin >> MAux::DAmpMin;
		cout <<"Distancia maxima entre Amplf. de Linha = ";
		cin >> MAux::DAmpMax;
		cout<<"#Pontos no grafico = ";
		cin >> Npontos;
		MAux::DAmpPasso = (MAux::DAmpMax-MAux::DAmpMin)/(Npontos-1);
	}

	if (MAux::AvaliaOsnr==SIM) {
		cout << "Entre com a potencia de entrada, em dBm." << endl;
		cin>>op;
		MAux::Config->set_Pin(op);
		cout << "Entre com a potencia de referencia da fibra, em dBm." << endl;
		cin>>op;
		MAux::Config->set_Pref(op);
		if (MAux::escSim != Sim_DAmp) {
			cout<<"Entre com distancia entre os amplificadores (em km)."<<endl;
			cin>>op;
			MAux::Config->set_DistaA(op);
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
	cin >> MAux::ExpComp;
	if(MAux::ExpComp) {
		cout<<"Entre com o MAux::muC: ";
		cin>> MAux::muC;
		cout<<"Entre com o MAux::laE: ";
		cin >> MAux::laE;
	}

	long double nR;
	if (MAux::escSim != Sim_DAmp) {
		cout<<"Entre com o numero minimo de requisicoes bloqueadas que o programa tem de encontrar antes de terminar: ";
		cin>> nR;
		Def::setNumReqBloqMin(nR);
	}

	for(int f = 0; f <= Def::getSE()-Def::getSR()+1; f++)
		MAux::Metrica<<f<<"\t"<<1.0/(f+1)<<endl;
}

void RefreshNoise(Def *Config) {
	for (int i = 0; i< Def::getNnodes(); i++ ) {
		for (int j = 0; j < Def::getNnodes(); j++ ) {
			MAux::Caminho[i].at(j).recalcular(Config);
		}
	}
}

bool ReleaseSlot(const Route* route, int s, Def *Config) {
	int L_or, L_de;
	for(unsigned c = 0; c < route->getNhops(); c++) {
		L_or = route->getNode(c);
		L_de = route->getNode(c+1);
		assert(Config->Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] == true);
		Config->Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] = false;
	}
	return true;
}

void RemoveCon(Event* evt, Def *Config) {
	for(int s = evt->conexao->getFirstSlot(); s <= evt->conexao->getLastSlot(); s++) {
		assert( (s >= 0) && (s < Def::getSE()) );
		ReleaseSlot(evt->conexao->getRoute(), s, Config);
	}
	delete evt->conexao;
	delete evt;
}

void RequestCon(Event* evt, Def *Config) {
	int orN, deN, NslotsReq, NslotsUsed, si, nTaxa;
	SDPairReq(orN, deN);
	nTaxa = TaxaReq();
	if (MAux::escSim == Sim_DAmp | MAux::escSim == Sim_NSlots) {
		nTaxa = Def::get_numPossiveisTaxas() - 1;
	}
	Config->taxaTotal += Def::PossiveisTaxas[nTaxa];

   /*if (MAux::escSim == Sim_AlfaOtimizado)
		nTaxa = 4;*/

	//Para o conjunto de rotas fornecida pelo roteamento, tenta alocar a requisicao:
	Route *route;
	long double OSNR = 0;

	Config->numReq++;
	Config->numReq_Taxa[nTaxa]++;

	EsquemaDeModulacao Esquemas[numEsquemasDeModulacao] = { _64QAM, _16QAM, _4QAM };
	for (int Esq = 0; Esq < numEsquemasDeModulacao; Esq++) {
		evt->Esquema = Esquemas[Esq];
		NslotsReq = SlotsReq(nTaxa, evt);

		if(MAux::Alg_Routing == CSP)
			RWA::DijkstraFormas(orN, deN, NslotsReq, Config);
		if(MAux::Alg_Routing == CSP_Acum)
			RWA::DijkstraAcum(orN, deN, NslotsReq, Config);
		if(MAux::Alg_Routing == DJK_SPeFormas)
			RWA::DijkstraSPeFormas(orN,deN,NslotsReq, 0.01*Def::Alfa, Config);
		if(MAux::Alg_Routing == LOR_Modificado)
			RWA::LORModificado(orN, deN, NslotsReq, Config);
		if(MAux::Alg_Routing == PSO)
			RWA::DijkstraPSR(orN, deN, NslotsReq, Config);
		if(MAux::escSim == Sim_TreinoPSR)
			RWA::DijkstraPSR(orN, deN, NslotsReq, Config);
		if(MAux::Alg_Routing == DJK_RuidoEFormas)
			RWA::DijkstraRuidoeFormas(orN, deN, NslotsReq, 0.01*Def::Beta, evt->Esquema, Def::PossiveisTaxas[nTaxa], Config);

		for(unsigned int i = 0; i < MAux::AllRoutes[orN*Def::getNnodes()+deN].size(); i++) {
			route = MAux::AllRoutes[orN*Def::getNnodes()+deN].at(i); //Tenta a i-esima rota destinada para o par orN-deN
			NslotsUsed = 0;
			si = -1;
			TryToConnect(route, NslotsReq, NslotsUsed, si, Config);
			assert( (NslotsUsed == 0) || (NslotsUsed == NslotsReq) ); //Tirar isso aqui quando uma conexao puder ser atendida com um numero menor de slots que o requisitado
			if(NslotsUsed > 0) { //A conexao foi aceita
				assert(NslotsUsed <= NslotsReq && si >= 0 && si <= Def::getSE()-NslotsUsed);
                if (MAux::AvaliaOsnr==SIM) OSNR = AvaliarOSNR(route, Config);
				if (MAux::AvaliaOsnr==NAO || OSNR >= Def::getlimiarOSNR(evt->Esquema, Def::PossiveisTaxas[nTaxa])) { //aceita a conexao
				//Inserir a conexao na rede
					int L_or, L_de;
					for(unsigned c = 0; c < route->getNhops(); c++) {
						L_or = route->getNode(c);
						L_de = route->getNode(c+1);
						for(int s = si; s < si + NslotsUsed; s++) {
							assert(Config->Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] == false);
							Config->Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] = true;
							//Os slots sao marcados como ocupados
						}
					}

					Config->numHopsPerRoute += route->getNhops();
					Config->netOccupancy += NslotsUsed*route->getNhops();

					//Cria uma nova conexao
					long double Tempo = General::exponential(MAux::mu);
					Conexao *newConexao = new Conexao(*route, si, si + NslotsUsed - 1, MAux::simTime + Tempo);
					//Agendar um dos eventos possiveis para conexao (Expandir, contrair, cair, etc):
					Event *evt = new Event;
					evt->conexao = newConexao;
					DefineNextEventOfCon(evt);
					ScheduleEvent(evt);
					Config->tempoTotal_Taxa[nTaxa] += Tempo;
					Config->numReqAceit_Esquema[Esq] += 1;
					Config->taxaTotal_Esquema[Esq] += Def::PossiveisTaxas[nTaxa];
					break;
				} else { //conexao bloqueada por OSNR
					NslotsUsed = 0;
					if (Esq == numEsquemasDeModulacao - 1) {
                        AccountForBlockingOSNR(NslotsReq,NslotsUsed,Config);
					}
				}
			}
		}
		if (NslotsUsed != 0) break;
	}
	Config->numSlots_Req += NslotsReq;

	//Verifica quantas conexoes e quantos slots foram bloqueados
	AccountForBlocking(NslotsReq, NslotsUsed, nTaxa, Config);
	//Define o novo evento de chegada de requisicao
	long double IAT = General::exponential(MAux::laNet); //Inter-arrival time
	setReqEvent(evt, MAux::simTime + IAT);
	assert(evt->type == Req);
	ScheduleEvent(evt); //Reusa este mesmo objeto evt
}

void ScheduleEvent(Event *evt) {
	Event *evtAux = MAux::firstEvent, *evtAnt = NULL;
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
		MAux::firstEvent = evt;
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
	if (MAux::escSim == Sim_DAmp | MAux::escSim == Sim_NSlots) {
		evt->Esquema = _4QAM;
	}
}

long double Simula_Rede(Def *Config) {
	clearMemory(Config);
	for (int i = 0; i < Def::Nnodes*Def::Nnodes; i++) {
		while (!MAux::AllRoutes[i].empty()) {
			delete MAux::AllRoutes[i].back();
			MAux::AllRoutes[i].pop_back();
		}
		vector<Route*> ().swap(MAux::AllRoutes[i]);
	}
	delete[] MAux::AllRoutes;
	MAux::AllRoutes = new vector<Route*> [Def::Nnodes*Def::Nnodes];
	MAux::firstEvent = new Event;
	setReqEvent(MAux::firstEvent, MAux::simTime);
	while((Config->numReq_Bloq < Def::getNumReqBloqMin()) && (Config->numReq < Def::getNumReqMax())) {
		Event *curEvent = MAux::firstEvent;
		MAux::firstEvent = MAux::firstEvent->nextEvent;
		MAux::simTime = curEvent->time;
		if(curEvent->type == Req) {
			RequestCon(curEvent, Config);
		} else if(curEvent->type == Desc) {
			RemoveCon(curEvent, Config);
		}
	}
	long double PbReq = Config->numReq_Bloq/Config->numReq;
	return PbReq;
}

void SimAlfaBeta() {
	long double PbReq;
	if (MAux::escOtim == OtimizarAlfa)
		for (Def::Alfa = 0; Def::Alfa <= 100; Def::Alfa += 2) {
			RefreshNoise(MAux::Config);
			PbReq = Simula_Rede(MAux::Config);
			cout << "Alfa " << 0.01*Def::Alfa << "\tPbReq " << PbReq << endl;
			MAux::Resul << 0.01*Def::Alfa << "\t" << PbReq << endl;
		}
	else if (MAux::escOtim == OtimizarBeta)
		for (Def::Beta = 0; Def::Beta <= 100; Def::Beta += 2) {
			RefreshNoise(MAux::Config);
			PbReq = Simula_Rede(MAux::Config);
			cout << "Beta " << 0.01*Def::Beta << "\tPbReq " << PbReq << endl;
			MAux::Resul << 0.01*Def::Beta << "\t" << PbReq << endl;
		}
}

void SimPbReq() {
	for(MAux::laNet = MAux::LaNetMin; MAux::laNet <= MAux::LaNetMax; MAux::laNet += MAux::LaPasso) {
		Sim();
		//SimCompFFO(); Simula usando as listas FF otimizadas
	}
}

void SimOSNR() {
	//Simulacao para varios trafegos
	cout << "Insira a carga da rede." << endl;
	cin >> MAux::laNet;
	for(long double osnr = MAux::OSNRMin; osnr <= MAux::OSNRMax; osnr += MAux::OSNRPasso) {
		MAux::Config->setOSNR(osnr);
		Sim();
		//SimCompFFO(); Simula usando as listas FF otimizadas
	}
}

void SimNSlots(Def *Config) {
	int SlotsMin, SlotsMax, SlotsPasso;
	cout << "Insira a carga da rede." << endl;
	cin >> MAux::laNet;
	cout << "Insira o número mínimo de slots por enlace" << endl;
	cin >> SlotsMin;
	SlotsPasso = SlotsMin;
	cout << "Insira o número máximo de slots por enlace" << endl;
	cin >> SlotsMax;
	for (int Slots = SlotsMin; Slots <= SlotsMax; Slots+=SlotsMin) {
		delete []Config->Topology_S;
		Def::setSE(Slots);
		Config->Topology_S = new bool[Def::getSE()*Def::Nnodes*Def::Nnodes]; //matriz de ocupação de slots de cada enlace
		for (int i = 0; i < Def::getSE()*Def::Nnodes*Def::Nnodes; i++)
			Config->Topology_S[i] = false;
		Simulate(Config);
	}
}


void Sim() {
	//Indica como o trafego e distribuido entre s = 1, 2, ..., SE
	MAux::Config->setLaUniform(MAux::laNet);
	//Def::setLaRandom(laNet);

	if(MAux::Alg_Aloc == FFO) {
		//Indica uma Heuristica:
		Heuristics::FFO_convencional(MAux::FFlists);
		//Heuristics::FFO_extremos(FFlists);
		//Heuristics::FFO_invertido(FFlists);
		//Heuristics::FFO_metrica(FFlists);
	}
	Simulate(MAux::Config);
}

void SimCompFFO() {
	long double Pb_conv, Pb_ext, Pb_met;
	long double difPerc, difPerc_FFOmet_FFOext_Pos = 0.0, difPerc_FFOmet_FFOext_Neg = 0.0,	difPerc_FFOconv_FFOext_Pos = 0.0, difPerc_FFOconv_FFOext_Neg = 0.0;
	for(int it = 0; it < 100; it++) {
		MAux::Config->setLaRandom(MAux::laNet);

		//Testa as heuristicas
		Heuristics::FFO_convencional(MAux::FFlists);
		Simulate(MAux::Config);
		Pb_conv = MAux::Config->numReq_Bloq/MAux::Config->numReq;

		Heuristics::FFO_extremos(MAux::FFlists);
		Simulate(MAux::Config);
		Pb_ext = (double)MAux::Config->numReq_Bloq/MAux::Config->numReq;

		Heuristics::FFO_metrica(MAux::FFlists);
		Simulate(MAux::Config);
		Pb_met = MAux::Config->numReq_Bloq/MAux::Config->numReq;

		//Calcular as diferencas percentuais:
		//Entre FFOmetrica e FFOextremos
		if( (Pb_met > 1000*(1.0/MAux::Config->numReq)) && (Pb_ext > 1000*(1.0/MAux::Config->numReq)) ) {
			//Tenho confianca nos resultados
			difPerc = (Pb_met - Pb_ext)/Pb_ext;
			if(difPerc > difPerc_FFOmet_FFOext_Pos) {
				difPerc_FFOmet_FFOext_Pos = difPerc;
				for(int Lr = 1; Lr <= Def::getSR(); Lr++)
					MAux::ResulFFOmet_FFOext<<MAux::Config->getLaNet(Lr)<<" ";
				MAux::ResulFFOmet_FFOext<<endl;
			}
			if(difPerc < difPerc_FFOmet_FFOext_Neg) {
				difPerc_FFOmet_FFOext_Neg = difPerc;
				for(int Lr = 1; Lr <= Def::getSR(); Lr++)
					MAux::ResulFFOext_FFOmet<<MAux::Config->getLaNet(Lr)<<" ";
				MAux::ResulFFOext_FFOmet<<endl;
			}

			difPerc = (Pb_conv - Pb_ext)/Pb_ext;
			if(difPerc > difPerc_FFOconv_FFOext_Pos) {
				difPerc_FFOconv_FFOext_Pos = difPerc;
				for(int Lr = 1; Lr <= Def::getSR(); Lr++)
					MAux::ResulFFOconv_FFOext<<MAux::Config->getLaNet(Lr)<<" ";
				MAux::ResulFFOconv_FFOext<<endl;
			}
			if(difPerc < difPerc_FFOconv_FFOext_Neg) {
				difPerc_FFOconv_FFOext_Neg = difPerc;
				for(int Lr = 1; Lr <= Def::getSR(); Lr++)
					MAux::ResulFFOext_FFOconv<<MAux::Config->getLaNet(Lr)<<" ";
				MAux::ResulFFOext_FFOconv<<endl;
			}
		}
	}

	cout<<"Maxima diferenca percentual Positiva entre FFOmet e FFOext: "<<difPerc_FFOmet_FFOext_Pos<<endl;
	cout<<"Maxima diferenca percentual Negativa entre FFOmet e FFOext: "<<difPerc_FFOmet_FFOext_Neg<<endl;
	cout<<"Maxima diferenca percentual Positiva entre FFOext e FFOconv: "<<difPerc_FFOconv_FFOext_Pos<<endl;
	cout<<"Maxima diferenca percentual Negativa entre FFOext e FFOconv: "<<difPerc_FFOconv_FFOext_Neg<<endl;
	MAux::Resul2<<"Maxima diferenca percentual Positiva entre FFOmet e FFOext: "<<difPerc_FFOmet_FFOext_Pos<<endl;
	MAux::Resul2<<"Maxima diferenca percentual Negativa entre FFOmet e FFOext: "<<difPerc_FFOmet_FFOext_Neg<<endl;
	MAux::Resul2<<"Maxima diferenca percentual Positiva entre FFOext e FFOconv: "<<difPerc_FFOconv_FFOext_Pos<<endl;
	MAux::Resul2<<"Maxima diferenca percentual Negativa entre FFOext e FFOconv: "<<difPerc_FFOconv_FFOext_Neg<<endl;
}

void Simulate(Def *Config) {
    clearMemory(Config);
	//Cria o primeiro evento da rede como uma requisicao:
	MAux::firstEvent = new Event;
	setReqEvent(MAux::firstEvent, MAux::simTime);
	while((Config->numReq_Bloq < Def::getNumReqBloqMin()) && (Config->numReq < Def::getNumReqMax())) {
		Event *curEvent = MAux::firstEvent;
		MAux::firstEvent = MAux::firstEvent->nextEvent;
		MAux::simTime = curEvent->time;
		if(curEvent->type == Req) {
			RequestCon(curEvent, Config);
		} else if(curEvent->type == Desc) {
			RemoveCon(curEvent, Config);
		} else if(curEvent->type == Exp) {
			assert(MAux::ExpComp); //Um evento deste tipo so pode ocorrer se MAux::ExpComp=true;
			ExpandCon(curEvent, Config);
		} else if(curEvent->type == Comp) {
			assert(MAux::ExpComp); //Um evento deste tipo so pode ocorrer se MAux::ExpComp=true;
			CompressCon(curEvent, Config);
		}
	}

	cout <<"Simulation Time= " << MAux::simTime << "  numReq=" << Config->numReq << endl;

	if (MAux::escSim == Sim_PbReq) {
		cout << "nu0= " << MAux::laNet << "   PbReq= " << ProbBloqueio(Config) << "   PbAc= " << ProbAceitacao(Config) << "   PbSlots= " << (long double) Config->numSlots_Bloq/Config->numSlots_Req << " HopsMed= " << (long double) Config->numHopsPerRoute/(Config->numReq-Config->numReq_Bloq) << " netOcc= " << (long double) Config->netOccupancy << endl;
		MAux::Resul << MAux::laNet << "\t" << (long double) Config->numReq_Bloq/Config->numReq << "\t" << (long double) (1.0 - Config->numReq_Bloq/Config->numReq) << "\t" << (long double) Config->numSlots_Bloq/Config->numSlots_Req << "\t" << (long double) Config->numHopsPerRoute/(Config->numReq-Config->numReq_Bloq) << "\t" << Config->netOccupancy << endl;
		MAux::ResulOSNR << MAux::laNet << "\t" << Config->numReq_BloqPorOSNR/Config->numReq_Bloq << endl;
		MAux::Resul2 << MAux::laNet << "\t" << Config->numReq_BloqPorOSNR/Config->numReq << "\t" << (1.0 - Config->numReq_BloqPorOSNR/Config->numReq_Bloq)*Config->numReq_Bloq/Config->numReq << endl;
	}

	else if (MAux::escSim == Sim_OSNR) {
		cout << "OSNR = " << Config->get_OSRNin() << "   PbReq= " << ProbBloqueio(Config) << "   PbAc= " << ProbAceitacao(Config) << "   PbSlots= " << (long double) Config->numSlots_Bloq/Config->numSlots_Req << " HopsMed= " << (long double) Config->numHopsPerRoute/(Config->numReq-Config->numReq_Bloq) << " netOcc= " << (long double) Config->netOccupancy << endl;
		MAux::Resul << Config->get_OSRNin() << "\t" << (long double) Config->numReq_Bloq/Config->numReq << "\t" << (long double) Config->numSlots_Bloq/Config->numSlots_Req << "\t" << (long double) Config->numHopsPerRoute/(Config->numReq-Config->numReq_Bloq) << "\t" << Config->netOccupancy << endl;
		MAux::ResulOSNR << Config->get_OSRNin() << "\t" << Config->numReq_BloqPorOSNR/Config->numReq_Bloq << endl;
	}

	else if (MAux::escSim == Sim_NSlots) {
		cout << "NSlots = " << Def::getSE() << "\t PbReq Fis = " << Config->numReq_BloqPorOSNR/Config->numReq << "\t PbReq Rede = " << (1.0 - Config->numReq_BloqPorOSNR/Config->numReq_Bloq)*Config->numReq_Bloq/Config->numReq << endl;
		MAux::Resul << Def::getSE() << "\t" << Config->numReq_Bloq/Config->numReq << "\t" << Config->numReq_BloqPorOSNR/Config->numReq << "\t" << (1.0 - Config->numReq_BloqPorOSNR/Config->numReq_Bloq)*Config->numReq_Bloq/Config->numReq << endl;
	}

	{
		ProbBloqueioTaxa(Config);
		ProbAceitacaoTaxa(Config);
		calcTaxaMedia(Config);
		AceitacaoEsquema(Config);
	}
}

void Simulate_dAMP(Def *Config) {
	RWA::DijkstraSP();
	Event *evt = new Event;	setReqEvent(evt,0);
	cout << "Limiar: " << Def::getlimiarOSNR(evt->Esquema,400E9) << "dB" << endl;
	for(long double osnr = MAux::OSNRMin; osnr <= MAux::OSNRMax; osnr += MAux::OSNRPasso) {
		Config->setOSNR(osnr);
		for (long double dAmplif = MAux::DAmpMin; dAmplif <= MAux::DAmpMax; dAmplif += MAux::DAmpPasso) {
			Config->set_DistaA(dAmplif);
			RefreshNoise(Config);
			setReqEvent(evt,0);
			long double Max = MAux::MinimasDistancias[0], OSNRout;
			int orN, deN;
			for (int i = 0; i < Def::getNnodes(); i++) {
				for (int j = 0; j < Def::getNnodes(); j++) {
					if (Max < MAux::MinimasDistancias[i*Def::Nnodes + j]) {
						Max = MAux::MinimasDistancias[i*Def::Nnodes + j];
						orN = i;
						deN = j;
					}
				}
			} //Encontra a maior entre as menores distancias
            OSNRout = AvaliarOSNR( MAux::AllRoutes[orN*Def::getNnodes() + deN].at(0) , Config);
			cout << "OSNRin = " << Config->get_OSRNin() << "dB, dAmp = " << Config->get_DistaA() << "km, OSNR = " << OSNRout << "dB" << endl; //primeira rota
			if ( OSNRout < Def::getlimiarOSNR(evt->Esquema,400E9) ) {
				MAux::ResultDAmpMenorQueLimiar << Config->get_DistaA() << "\t" << Config->get_OSRNin() << endl;
			} else {
				MAux::ResultDAmpMaiorQueLimiar << Config->get_DistaA() << "\t" << Config->get_OSRNin() << endl;
			}
		}
	}
	delete evt;
}

void EncontraMultiplicador(Def *Config) {
	RWA::DijkstraSP();
	Event *evt = new Event;	setReqEvent(evt,0);
	Config->set_DistaA(80); //80km
	Config->setOSNR(30); //30dB

	cout << "Limiar: " << Def::getlimiarOSNR(evt->Esquema,400E9) << "dB" << endl;

	int orN, deN;
	long double Max = MAux::MinimasDistancias[0];
	for (int i = 0; i < Def::getNnodes(); i++) {
		for (int j = 0; j < Def::getNnodes(); j++) {
			if (Max < MAux::MinimasDistancias[i*Def::Nnodes + j]) {
				Max = MAux::MinimasDistancias[i*Def::Nnodes + j];
				orN = i;
				deN = j;
			}
		}
	} //Encontra a maior entre as menores distancias

	for (long double multiplicador = 1; multiplicador <= 30; multiplicador += 1) {

		if (multiplicador != 1)
			for (int i = 0; i < MAux::AllRoutes[orN*Def::Nnodes + deN].at(0)->getNhops(); i++) {
				int noOrig = MAux::AllRoutes[orN*Def::Nnodes + deN].at(0)->getNode(i);
				int noDest = MAux::AllRoutes[orN*Def::Nnodes + deN].at(0)->getNode(i+1);
				long double oldDist = MAux::Caminho[noOrig].at(noDest).get_comprimento();
				long double newDist = oldDist * multiplicador / (multiplicador - 1);
				MAux::Caminho[noOrig].at(noDest).set_distancia(newDist);
				MAux::Caminho[noDest].at(noOrig).set_distancia(newDist);
			}

		RefreshNoise(Config);
		setReqEvent(evt,0);
		long double OSNRout;
        OSNRout = AvaliarOSNR( MAux::AllRoutes[orN*Def::getNnodes() + deN].at(0) , Config);
		cout << "Multiplicador = " << multiplicador << ", OSNR = " << OSNRout << "dB" << endl; //primeira rota
	}
	delete evt;
}

int SlotsReq(int Ran, Event *evt) {
	return ceil(Def::PossiveisTaxas[Ran]/(2*log2(evt->Esquema)*Def::get_Bslot()));
}

int TaxaReq() {
	return floor( General::uniforme(0.0,Def::get_numPossiveisTaxas()) );
}

void TryToConnect(const Route* route, const int NslotsReq, int& NslotsUsed, int& si, Def *Config) {
	//NslotsReq informa a quantidade de slots requisitados para a conexao;
	//si informa o slot inicial da sequencia de slots utilizada;
	si = -1, NslotsUsed = 0; //1 <= NslotsUsed <= Nslots informa quantos slots sao utilizados para a conexao
	switch(MAux::Alg_Aloc) {
		case RD:
			RWA::Random(route, NslotsReq, NslotsUsed, si, Config);
			break;
		case FF:
			RWA::FirstFit(route, NslotsReq, NslotsUsed, si, Config);
			break;
		case MU:
			RWA::MostUsed(route, NslotsReq, NslotsUsed, si, Config);
			break;
		case FFO:
			RWA::FirstFitOpt(route, NslotsReq, NslotsUsed, si, Config);
			break;
		default:
			cout<<"Algoritmo nao definido"<<endl;
	}
}
