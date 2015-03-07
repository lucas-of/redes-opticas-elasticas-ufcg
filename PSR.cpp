#include "PSR.h"
#include "Main_Auxiliar.h"
int PSR::N;
long double **PSR::Coeficientes, **PSR::ComprimentosNormalizados;
Particula *PSR::PSO_populacao;
long double PSR::MaiorEnlace = -1;
int PSR::PSO_P, PSR::PSO_G;
long double PSR::PSO_c1, PSR::PSO_c2, PSR::PSO_chi, PSR::PSO_MelhorPbReq = 1;
ofstream PSR::PSO_Coeficientes_W("PSOCoeficientes.txt");
ifstream PSR::PSO_Coeficientes_R("PSOCoeficientes.txt");

void clearMemory(); /*Limpa e zera todas as constantes de Def.h, reinicia o tempo de simulação e libera todos os slots.*/
void RemoveCon(Event*); /*Retira uma conexão da rede - liberando todos os seus slots*/
void RequestCon(Event*); /*Cria uma conexão. Dados dois nós, procura pelo algoritmo de roteamento definido uma rota entre os mesmos. Após encontrar a rota, cria a conexão, e por fim agenda o próximo evento de requisição de conexão.*/
void setReqEvent(Event*, TIME); /*Cria um evento de requisição a partir do instante de criação (TIME)*/

PSR::PSR(int NewN) {
	assert(NewN > 0);
	MaiorEnlace = 0;
	N = NewN;

	Coeficientes = new long double*[PSR::get_N()];
	for (int i = 0; i < N; i++) Coeficientes[i] = new long double[PSR::get_N()];

	ComprimentosNormalizados = new long double*[Def::getNnodes()];
	for (int i = 0; i < Def::getNnodes(); i++) {
		ComprimentosNormalizados[i] = new long double[Def::getNnodes()];
	}
	Normalizacao();
}

PSR::~PSR() {
	delete[] Coeficientes;
}

const int PSR::get_N() {
	return PSR::N;
}

long double PSR::get_coeficiente(int i, int j) {
	assert ((i < get_N()) && (i >= 0));
	assert ((j < get_N()) && (j >= 0));
	return Coeficientes[i][j];
}

long double PSR::procurarMaiorEnlace() {
	for (int i = 0; i < Def::getNnodes(); i++) {
		for (int j = 0; j < Def::getNnodes(); j++) {

			if (MAux::Caminho[i].at(j).get_NodeOrigem() == NULL) continue;
			if (MaiorEnlace < MAux::Caminho[i].at(j).get_comprimento()) MaiorEnlace = MAux::Caminho[i].at(j).get_comprimento();
		}
	}
	return MaiorEnlace;
}

void PSR::Normalizacao() {
	procurarMaiorEnlace();

	for (int i = 0; i < Def::getNnodes(); i++) {
		for (int j = 0; j < Def::getNnodes(); j++) {
			if (MAux::Topology[i][j] == 0) {
				ComprimentosNormalizados[i][j] = Def::MAX_LONGDOUBLE;
				continue;
			}
			ComprimentosNormalizados[i][j] = MAux::Caminho[i].at(j).get_comprimento() / MaiorEnlace;
		}
	}
}

void PSR::PSO_configurar() {
	PSO_P = 50;
	PSO_G = 50;
	PSO_c1 = 2.05;
	PSO_c2 = 2.05;

	long double phi = PSO_c1 + PSO_c2;
	PSO_chi = -2.0/(2 - phi - sqrt(phi*phi - 4*phi));

	PSO_populacao = new Particula[PSO_P];
	for (int i = 0; i < PSO_P; i++) {
		if (i!=0) PSO_populacao[i].Vizinha1 = PSO_populacao + i - 1;
		else PSO_populacao[i].Vizinha1 = PSO_populacao + PSO_P - 1;
		if(i!=PSO_P-1) PSO_populacao[i].Vizinha2 = PSO_populacao + i + 1;
		else PSO_populacao[i].Vizinha2 = PSO_populacao;

		PSO_populacao[i].x = new long double*[N];
		PSO_populacao[i].v = new long double*[N];
		PSO_populacao[i].p = new long double*[N];
		for (int j = 0; j < N; j++) {
			PSO_populacao[i].x[j] = new long double[N];
			PSO_populacao[i].v[j] = new long double[N];
			PSO_populacao[i].p[j] = new long double[N];
		}
	}
}

void PSR::PSO() {
	long double PbReq;

	PSO_configurar();
	PSO_iniciarPopulacao();

	for (int Repeticao = 0; Repeticao < PSO_G; Repeticao++) {
		cout << "PSO - Repeticao " << Repeticao << "." << endl;
		for (int Part = 0; Part < PSO_P; Part++) {
			PbReq = PSO_simulaRede(PSO_populacao + Part);
			if (PbReq < (PSO_populacao + Part)->melhorInd) {
				(PSO_populacao + Part)->melhorInd = PbReq;
				(PSO_populacao + Part)->p = (PSO_populacao + Part)->x;
			}
			if (PbReq < PSR::PSO_MelhorPbReq) {
				PSR::PSO_MelhorPbReq = PbReq;
				for (int i = 0; i < N; i++)
					for (int j = 0; j < N; j++)
						Coeficientes[i][j] = (PSO_populacao+Part)->x[i][j];
			}
			cout << "Particula " << Part << " PbReq " << PbReq << " (" << PSO_MelhorPbReq << ")" << endl;
		}
		PSO_atualizaVelocidades();
	}
}

void PSR::PSO_iniciarPopulacao() {
	for (int i = 0; i < PSO_P; i++) {
		for (int j = 0; j < N; j++) {
			for (int k = 0; k < N; k++) {
				PSO_populacao[i].v[j][k] = 0;
				PSO_populacao[i].x[j][k] = General::uniforme(0,1);
			}
		}
	}
}

void PSR::PSO_atualizaCustoEnlaces(Particula *P) {
	for (int i = 0; i < Def::getNnodes(); i++) {
		for (int j = 0; j < Def::getNnodes(); j++) {
			if (MAux::Topology[i][j] == 1)
				MAux::Caminho[i].at(j).recalcular_peso(P->x);
		}
	}
}

long double PSR::get_MaiorEnlace() {
	if (MaiorEnlace==-1) procurarMaiorEnlace();
	return MaiorEnlace;
}

long double PSR::PSO_simulaRede(Particula *P) {
	PSO_atualizaCustoEnlaces(P);
	clearMemory();
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
	return PbReq;
}

void PSR::executar_PSR() {
	PSO();
	PSO_ImprimeCoeficientes();
}

void PSR::PSO_atualizaVelocidades() {
	Particula *Fitter;
	long double eps1, eps2;
	for (int i = 0; i < PSO_P; i++) {
		if (PSO_populacao[i].Vizinha1->melhorInd < PSO_populacao[i].Vizinha2->melhorInd)
			Fitter = PSO_populacao[i].Vizinha2;
		else
			Fitter = PSO_populacao[i].Vizinha1;

		for (int j = 0; j < N; j++) {
			for (int k = 0; k < N; k++) {
				eps1 = General::uniforme(0,1);
				eps2 = General::uniforme(0,1);
				PSO_populacao[i].v[j][k] = PSO_chi * ( PSO_populacao[i].v[j][k] + PSO_c1*eps1*( PSO_populacao[i].p[j][k] - PSO_populacao[i].x[j][k] ) + PSO_c2*eps2*( Fitter->p[j][k] - PSO_populacao[i].x[j][k] ) );
				PSO_populacao[i].x[j][k] += PSO_populacao[i].v[j][k];
			}
		}
	}
}

void PSR::PSO_ImprimeCoeficientes() {
	PSO_Coeficientes_W << N << endl;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			PSO_Coeficientes_W << Coeficientes[i][j] << "\t";
		}
		PSO_Coeficientes_W << endl;
	}
	PSO_Coeficientes_W << endl << PSO_MelhorPbReq << endl;
}
