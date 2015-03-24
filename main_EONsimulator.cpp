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
void AccountForBlocking(int NslotsReq, int NslotsUsed, int nTaxa); /*Realiza ações necessárias para quando uma conexão foi bloqueada*/
void AccountForBlockingOSNR(int NslotsReq, int NslotsUsed); /*Realiza ações necessárias para quando uma conexão foi bloqueada*/
long double AvaliarOSNR(const Route *Rota, int NSlotsUsed); /*avalia a ONSR da routa passada como parâmetro*/
bool checkFitSi(const bool* vetDisp, int s, int NslotsReq); /*Indica se a conexao pode ser inserida em [s:s+NslotsReq]*/
void clearMemory(); /*Limpa e zera todas as constantes de Def.h, reinicia o tempo de simulação e libera todos os slots.*/
void CarregaCoeficientes();
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
long double Simula_Rede();
void SimPbReq(); /*Simulação para Probabilidade de Bloqueio*/
void SimOSNR(); /*Simulação para OSNR*/
void SimNSlots();
void SimAlfa();
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
	if (MAux::Alg_Routing == DJK)
		RWA::Dijkstra();
	else if (MAux::Alg_Routing == SP)
		RWA::DijkstraSP();
	if (MAux::escSim == Sim_PbReq)
		SimPbReq();
	else if (MAux::escSim == Sim_OSNR)
		SimOSNR();
	else if (MAux::escSim == Sim_DAmp)
		Simulate_dAMP();
	else if (MAux::escSim == Sim_NSlots)
		SimNSlots();
	else if (MAux::escSim == Sim_TreinoPSR) {
		PSR(3);
		PSR::executar_PSR();
	} else if (MAux::escSim == Sim_AlfaOtimizado) {
		SimAlfa();
	}

	delete []MAux::Topology;
	delete []MAux::Topology_S;
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

void AccountForBlocking(int NslotsReq, int NslotsUsed, int nTaxa) {
	if(NslotsUsed <= 0) { //A conexao foi bloqueada
		Def::numReq_Bloq++;
		Def::numReqBloq_Taxa[nTaxa]++;
	}
	Def::numSlots_Bloq += (NslotsReq - NslotsUsed);
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

void clearMemory() {
	int s, L_or, L_de;
	Conexao *con;
	Event *evtPtr;
	const Route* route;
	Def::numReq = Def::numReq_Bloq = Def::numSlots_Req = Def::numSlots_Bloq = Def::numHopsPerRoute = Def::netOccupancy = MAux::simTime = Def::numReq_BloqPorOSNR = Def::numSlots_BloqPorOSNR = 0.0;
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
					MAux::Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] = false;
			}
			delete con;
		}
		evtPtr = MAux::firstEvent;
		MAux::firstEvent = MAux::firstEvent->nextEvent;
		delete evtPtr;
	}

	for (int i = 0; i<Def::get_numPossiveisTaxas(); i++) {
		Def::numReqBloq_Taxa[i] = 0;
		Def::numReq_Taxa[i] = 0;
		Def::tempoTotal_Taxa[i] = 0;
	}

	for (int i = 0; i<Def::numEsquemasDeModulacao; i++) {
		Def::numReqAceit_Esquema[i] = 0;
		Def::taxaTotal_Esquema[i] = 0;
	}

	//Checar se limpeza foi realizada corretamente
	for(int orN = 0; orN < Def::getNnodes(); orN++)
		for(int deN = 0; deN < Def::getNnodes(); deN++)
			for(s = 0; s < Def::getSE(); s++)
				assert(MAux::Topology_S[s*Def::Nnodes*Def::Nnodes + orN*Def::Nnodes + deN] == false);
	assert(MAux::firstEvent == NULL);
}

void CompressCon(Event* evt) {
	if (MAux::ExpComp) {
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
	MAux::Topology = new long double[Def::Nnodes*Def::Nnodes]; //matriz de conexões entre nós
	MAux::Topology_S = new bool[Def::Nnodes*Def::Nnodes*Def::getSE()]; //matriz de ocupação de slots de cada enlace
	for (int i=0 ; i < Def::getSE(); i++)
		for (int j=0; j < Def::getNnodes() ; j++)
			for (int k = 0; k < Def::getNnodes(); k++)
				MAux::Topology_S[i*Def::Nnodes*Def::Nnodes + Def::Nnodes*j + k] = false;

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
	GrauDosNodes();

	for (int i=0;i < Def::getNnodes() ; i++) {
		MAux::Rede.push_back(Node(i));
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
				MAux::Caminho[i].push_back(Enlace(&MAux::Rede.at(i),&MAux::Rede.at(j),10*distancia_temp));
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


void ExpandCon(Event* evt) {
	if (MAux::ExpComp) {
		Conexao *con = evt->conexao;
		//Procura um slot lateral. Assume-se que tenta primeiro o slot a esquerda e depois o a direita.
		if(con->getFirstSlot() > 0 && RWA::CheckSlotAvailability(con->getRoute(), con->getFirstSlot()-1)) {
			FillSlot(con->getRoute(), con->getFirstSlot()-1, true);
			con->incFirstSlot(-1);
		} else if(con->getLastSlot()<Def::getSE()-1 && RWA::CheckSlotAvailability(con->getRoute(),con->getLastSlot()+1)) {
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
		assert(MAux::Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] != b);
		MAux::Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] = b;
	}
	return true;
}

void GrauDosNodes() {
	int node_temp = 0;
	Def::clearGrauNo();
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

	cout << "Escolha a Simulação. " << endl << "\tProbabilidade de Bloqueio <" << Sim_PbReq << ">;" << endl << "\tOSNR <" << Sim_OSNR << ">; " << endl << "\tDistancia dos Amplificadores <" << Sim_DAmp << ">;" << endl << "\tNumero de Slots <" << Sim_NSlots << ">;" << endl << "\tPSR - Otimização <" << Sim_TreinoPSR << ">;" << endl << "\tOtimização do Alfa <" << Sim_AlfaOtimizado << ">." << endl;
	cin >> aux;
	MAux::escSim = (Simulacao)aux;

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

	if ((MAux::escSim != Sim_TreinoPSR) && (MAux::escSim != Sim_AlfaOtimizado)) {
		cout << "\t" << DJK<<" - DJK \n\t"<<DJK_Formas<<" - DJK_Formas \n\t"<< DJK_Acum<<" - DijkstraAcumulado\n\t" << SP << " - Shortest Path\n\t"<< DJK_SPeFormas << " - DJK Shortest Path e Formas\n\t" << LOR_Modificado << " - LOR Modificado\n\t" << PSO << " - PSO\n";
		cout << "Entre com o Algoritmo de Roteamento: ";
		cin >> MAux::Alg_Routing;
	}
	if (MAux::escSim == Sim_AlfaOtimizado) {
		MAux::Alg_Routing = DJK_SPeFormas;
	}

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
	if (MAux::escSim == Sim_TreinoPSR || MAux::escSim == Sim_AlfaOtimizado) {
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
		Def::set_Pin(op);
		cout << "Entre com a potencia de referencia da fibra, em dBm." << endl;
		cin>>op;
		Def::set_Pref(op);
		if (MAux::escSim != Sim_DAmp) {
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
	cin >> MAux::ExpComp;
	if(MAux::ExpComp) {
		cout<<"Entre com o MAux::muC: ";
		cin>> MAux::muC;
		cout<<"Entre com o MAux::laE: ";
		cin >> MAux::laE;
	}

	long double nR;
	if (MAux::escSim != Sim_DAmp) {
		cout<<"Entre com o numero maximo de requisicoes que o programa vai rodar antes de terminar: ";
		cin>> nR;
		Def::setNumReqMax(nR);
	}

	for(int f = 0; f <= Def::getSE()-Def::getSR()+1; f++)
		MAux::Metrica<<f<<"\t"<<1.0/(f+1)<<endl;
}

void RefreshNoise() {
	for (int i = 0; i< Def::getNnodes(); i++ ) {
		for (int j = 0; j < Def::getNnodes(); j++ ) {
			MAux::Caminho[i].at(j).recalcular();
		}
	}
}

bool ReleaseSlot(const Route* route, int s) {
	int L_or, L_de;
	for(unsigned c = 0; c < route->getNhops(); c++) {
		L_or = route->getNode(c);
		L_de = route->getNode(c+1);
		assert(MAux::Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] == true);
		MAux::Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] = false;
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
	if (MAux::escSim == Sim_DAmp | MAux::escSim == Sim_NSlots) {
		nTaxa = Def::get_numPossiveisTaxas() - 1;
	}

   /*if (MAux::escSim == Sim_AlfaOtimizado)
		nTaxa = 4;*/

	//Para o conjunto de rotas fornecida pelo roteamento, tenta alocar a requisicao:
	Route *route;
	long double OSNR = 0;

	Def::numReq++;
	Def::numReq_Taxa[nTaxa]++;

	EsquemaDeModulacao Esquemas[numEsquemasDeModulacao] = { _64QAM, _16QAM, _4QAM };
	for (int Esq = 0; Esq < numEsquemasDeModulacao; Esq++) {
		evt->Esquema = Esquemas[Esq];
		NslotsReq = SlotsReq(nTaxa, evt);

		if(MAux::Alg_Routing == DJK_Formas)
			RWA::DijkstraFormas(orN, deN, NslotsReq);
		if(MAux::Alg_Routing == DJK_Acum)
			RWA::DijkstraAcum(orN, deN, NslotsReq);
		if(MAux::Alg_Routing == DJK_SPeFormas)
			RWA::DijkstraSPeFormas(orN,deN,NslotsReq, 0.1*Def::Alfa);
		if(MAux::Alg_Routing == LOR_Modificado)
			RWA::LORModificado(orN, deN, NslotsReq);
		if(MAux::Alg_Routing == PSO)
			RWA::DijkstraPSR(orN, deN, NslotsReq);
		if(MAux::escSim == Sim_TreinoPSR)
			RWA::DijkstraPSR(orN, deN, NslotsReq);

		for(unsigned int i = 0; i < MAux::AllRoutes[orN*Def::getNnodes()+deN].size(); i++) {
			route = MAux::AllRoutes[orN*Def::getNnodes()+deN].at(i); //Tenta a i-esima rota destinada para o par orN-deN
			NslotsUsed = 0;
			si = -1;
			TryToConnect(route, NslotsReq, NslotsUsed, si);
			assert( (NslotsUsed == 0) || (NslotsUsed == NslotsReq) ); //Tirar isso aqui quando uma conexao puder ser atendida com um numero menor de slots que o requisitado
			if(NslotsUsed > 0) { //A conexao foi aceita
				assert(NslotsUsed <= NslotsReq && si >= 0 && si <= Def::getSE()-NslotsUsed);
				if (MAux::AvaliaOsnr==SIM) OSNR = AvaliarOSNR(route,NslotsUsed);
				if (MAux::AvaliaOsnr==NAO || OSNR >= Def::getlimiarOSNR(evt->Esquema, Def::PossiveisTaxas[nTaxa])) { //aceita a conexao
				//Inserir a conexao na rede
					int L_or, L_de;
					for(unsigned c = 0; c < route->getNhops(); c++) {
						L_or = route->getNode(c);
						L_de = route->getNode(c+1);
						for(int s = si; s < si + NslotsUsed; s++) {
							assert(MAux::Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] == false);
							MAux::Topology_S[s*Def::Nnodes*Def::Nnodes + L_or*Def::Nnodes + L_de] = true;
							//Os slots sao marcados como ocupados
						}
					}

					Def::numHopsPerRoute += route->getNhops();
					Def::netOccupancy += NslotsUsed*route->getNhops();

					//Cria uma nova conexao
					long double Tempo = General::exponential(MAux::mu);
					Conexao *newConexao = new Conexao(*route, si, si + NslotsUsed - 1, MAux::simTime + Tempo);
					//Agendar um dos eventos possiveis para conexao (Expandir, contrair, cair, etc):
					Event *evt = new Event;
					evt->conexao = newConexao;
					DefineNextEventOfCon(evt);
					ScheduleEvent(evt);
					Def::tempoTotal_Taxa[nTaxa] += Tempo;
					Def::numReqAceit_Esquema[Esq] += 1;
					Def::taxaTotal_Esquema[Esq] += Def::PossiveisTaxas[nTaxa];
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

long double Simula_Rede() {
	clearMemory();
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
	while(Def::numReq < Def::getNumReqMax()) {
		Event *curEvent = MAux::firstEvent;
		MAux::firstEvent = MAux::firstEvent->nextEvent;
		MAux::simTime = curEvent->time;
		if(curEvent->type == Req) {
			RequestCon(curEvent);
		} else if(curEvent->type == Desc) {
			RemoveCon(curEvent);
		}
	}
	long double PbReq = Def::numReq_Bloq/Def::numReq;
	for (int i = 0; i < Def::get_numPossiveisTaxas(); i++)
		cout << i << " " << Def::numReqBloq_Taxa[i] << endl;
	return PbReq;
}

void SimAlfa() {
	long double PbReq;
	for (Def::Alfa = 0; Def::Alfa <= 10; Def::Alfa += 1) {
		RefreshNoise();
		PbReq = Simula_Rede();
		cout << "Alfa " << 0.1*Def::Alfa << "\tPbReq " << PbReq << endl;
		MAux::Resul << 0.1*Def::Alfa << "\t" << PbReq << endl;
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
		Def::setOSNR(osnr);
		Sim();
		//SimCompFFO(); Simula usando as listas FF otimizadas
	}
}

void SimNSlots() {
	int SlotsMin, SlotsMax, SlotsPasso;
	cout << "Insira a carga da rede." << endl;
	cin >> MAux::laNet;
	cout << "Insira o número mínimo de slots por enlace" << endl;
	cin >> SlotsMin;
	SlotsPasso = SlotsMin;
	cout << "Insira o número máximo de slots por enlace" << endl;
	cin >> SlotsMax;
	for (int Slots = SlotsMin; Slots <= SlotsMax; Slots+=SlotsMin) {
		delete []MAux::Topology_S;
		Def::setSE(Slots);
		MAux::Topology_S = new bool[Def::getSE()*Def::Nnodes*Def::Nnodes]; //matriz de ocupação de slots de cada enlace
		Simulate();
	}
}


void Sim() {
	//Indica como o trafego e distribuido entre s = 1, 2, ..., SE
	Def::setLaUniform(MAux::laNet);
	//Def::setLaRandom(laNet);

	if(MAux::Alg_Aloc == FFO) {
		//Indica uma Heuristica:
		Heuristics::FFO_convencional(MAux::FFlists);
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
		Def::setLaRandom(MAux::laNet);

		//Testa as heuristicas
		Heuristics::FFO_convencional(MAux::FFlists);
		Simulate();
		Pb_conv = Def::numReq_Bloq/Def::numReq;

		Heuristics::FFO_extremos(MAux::FFlists);
		Simulate();
		Pb_ext = (double)Def::numReq_Bloq/Def::numReq;

		Heuristics::FFO_metrica(MAux::FFlists);
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
					MAux::ResulFFOmet_FFOext<<Def::getLaNet(Lr)<<" ";
				MAux::ResulFFOmet_FFOext<<endl;
			}
			if(difPerc < difPerc_FFOmet_FFOext_Neg) {
				difPerc_FFOmet_FFOext_Neg = difPerc;
				for(int Lr = 1; Lr <= Def::getSR(); Lr++)
					MAux::ResulFFOext_FFOmet<<Def::getLaNet(Lr)<<" ";
				MAux::ResulFFOext_FFOmet<<endl;
			}

			difPerc = (Pb_conv - Pb_ext)/Pb_ext;
			if(difPerc > difPerc_FFOconv_FFOext_Pos) {
				difPerc_FFOconv_FFOext_Pos = difPerc;
				for(int Lr = 1; Lr <= Def::getSR(); Lr++)
					MAux::ResulFFOconv_FFOext<<Def::getLaNet(Lr)<<" ";
				MAux::ResulFFOconv_FFOext<<endl;
			}
			if(difPerc < difPerc_FFOconv_FFOext_Neg) {
				difPerc_FFOconv_FFOext_Neg = difPerc;
				for(int Lr = 1; Lr <= Def::getSR(); Lr++)
					MAux::ResulFFOext_FFOconv<<Def::getLaNet(Lr)<<" ";
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

void Simulate() {
	clearMemory();
	//Cria o primeiro evento da rede como uma requisicao:
	MAux::firstEvent = new Event;
	setReqEvent(MAux::firstEvent, MAux::simTime);
	while(Def::numReq < Def::getNumReqMax()) {
		Event *curEvent = MAux::firstEvent;
		MAux::firstEvent = MAux::firstEvent->nextEvent;
		MAux::simTime = curEvent->time;
		if(curEvent->type == Req) {
			RequestCon(curEvent);
		} else if(curEvent->type == Desc) {
			RemoveCon(curEvent);
		} else if(curEvent->type == Exp) {
			assert(MAux::ExpComp); //Um evento deste tipo so pode ocorrer se MAux::ExpComp=true;
			ExpandCon(curEvent);
		} else if(curEvent->type == Comp) {
			assert(MAux::ExpComp); //Um evento deste tipo so pode ocorrer se MAux::ExpComp=true;
			CompressCon(curEvent);
		}
	}

	cout <<"Simulation Time= " << MAux::simTime << "  numReq=" << Def::numReq << endl;

	if (MAux::escSim == Sim_PbReq) {
		cout << "nu0= " << MAux::laNet << "   PbReq= " << ProbBloqueio() << "   PbAc= " << ProbAceitacao() << "   PbSlots= " << (long double) Def::numSlots_Bloq/Def::numSlots_Req << " HopsMed= " << (long double) Def::numHopsPerRoute/(Def::numReq-Def::numReq_Bloq) << " netOcc= " << (long double) Def::netOccupancy << endl;
		MAux::Resul << MAux::laNet << "\t" << (long double) Def::numReq_Bloq/Def::numReq << "\t" << (long double) (1.0 - Def::numReq_Bloq/Def::numReq) << "\t" << (long double) Def::numSlots_Bloq/Def::numSlots_Req << "\t" << (long double) Def::numHopsPerRoute/(Def::numReq-Def::numReq_Bloq) << "\t" << Def::netOccupancy << endl;
		MAux::ResulOSNR << MAux::laNet << "\t" << Def::numReq_BloqPorOSNR/Def::numReq_Bloq << endl;
		MAux::Resul2 << MAux::laNet << "\t" << Def::numReq_BloqPorOSNR/Def::numReq << "\t" << (1.0 - Def::numReq_BloqPorOSNR/Def::numReq_Bloq)*Def::numReq_Bloq/Def::numReq << endl;
	}

	else if (MAux::escSim == Sim_OSNR) {
		cout << "OSNR = " << Def::get_OSRNin() << "   PbReq= " << ProbBloqueio() << "   PbAc= " << ProbAceitacao() << "   PbSlots= " << (long double) Def::numSlots_Bloq/Def::numSlots_Req << " HopsMed= " << (long double) Def::numHopsPerRoute/(Def::numReq-Def::numReq_Bloq) << " netOcc= " << (long double) Def::netOccupancy << endl;
		MAux::Resul << Def::get_OSRNin() << "\t" << (long double) Def::numReq_Bloq/Def::numReq << "\t" << (long double) Def::numSlots_Bloq/Def::numSlots_Req << "\t" << (long double) Def::numHopsPerRoute/(Def::numReq-Def::numReq_Bloq) << "\t" << Def::netOccupancy << endl;
		MAux::ResulOSNR << Def::get_OSRNin() << "\t" << Def::numReq_BloqPorOSNR/Def::numReq_Bloq << endl;
	}

	else if (MAux::escSim == Sim_NSlots) {
		cout << "NSlots = " << Def::getSE() << "\t PbReq Fis = " << Def::numReq_BloqPorOSNR/Def::numReq << "\t PbReq Rede = " << (1.0 - Def::numReq_BloqPorOSNR/Def::numReq_Bloq)*Def::numReq_Bloq/Def::numReq << endl;
		MAux::Resul << Def::getSE() << "\t" << Def::numReq_BloqPorOSNR/Def::numReq << "\t" << (1.0 - Def::numReq_BloqPorOSNR/Def::numReq_Bloq)*Def::numReq_Bloq/Def::numReq << endl;
	}

	{
		ProbBloqueioTaxa();
		ProbAceitacaoTaxa();
		calcTaxaMedia();
		AceitacaoEsquema();
	}
}

void Simulate_dAMP() {
	RWA::DijkstraSP();
	Event *evt = new Event;	setReqEvent(evt,0);
	cout << "Limiar: " << Def::getlimiarOSNR(evt->Esquema,400E9) << "dB" << endl;
	for(long double osnr = MAux::OSNRMin; osnr <= MAux::OSNRMax; osnr += MAux::OSNRPasso) {
		Def::setOSNR(osnr);
		for (long double dAmplif = MAux::DAmpMin; dAmplif <= MAux::DAmpMax; dAmplif += MAux::DAmpPasso) {
			Def::set_DistaA(dAmplif);
			RefreshNoise();
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
			OSNRout = AvaliarOSNR( MAux::AllRoutes[orN*Def::getNnodes() + deN].at(0) , SlotsReq(Def::get_numPossiveisTaxas() - 1, evt));
			cout << "OSNRin = " << Def::get_OSRNin() << "dB, dAmp = " << Def::get_DistaA() << "km, OSNR = " << OSNRout << "dB" << endl; //primeira rota
			if ( OSNRout < Def::getlimiarOSNR(evt->Esquema,400E9) ) {
				MAux::ResultDAmpMenorQueLimiar << Def::get_DistaA() << "\t" << Def::get_OSRNin() << endl;
			} else {
				MAux::ResultDAmpMaiorQueLimiar << Def::get_DistaA() << "\t" << Def::get_OSRNin() << endl;
			}
		}
	}
	delete evt;
}

int SlotsReq(int Ran, Event *evt) {
	return ceil(Def::PossiveisTaxas[Ran]/(2*log2(evt->Esquema)*Def::get_Bslot()));
}

int TaxaReq() {
	return floor( General::uniforme(0.0,Def::get_numPossiveisTaxas()) );
}

void TryToConnect(const Route* route, const int NslotsReq, int& NslotsUsed, int& si) {
	//NslotsReq informa a quantidade de slots requisitados para a conexao;
	//si informa o slot inicial da sequencia de slots utilizada;
	si = -1, NslotsUsed = 0; //1 <= NslotsUsed <= Nslots informa quantos slots sao utilizados para a conexao
	switch(MAux::Alg_Aloc) {
		case RD:
			RWA::Random(route, NslotsReq, NslotsUsed, si);
			break;
		case FF:
			RWA::FirstFit(route, NslotsReq, NslotsUsed, si);
			break;
		case MU:
			RWA::MostUsed(route, NslotsReq, NslotsUsed, si);
			break;
		case FFO:
			RWA::FirstFitOpt(route, NslotsReq, NslotsUsed, si);
			break;
		default:
			cout<<"Algoritmo nao definido"<<endl;
	}
}
