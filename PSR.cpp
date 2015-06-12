#include "PSR.h"
#include "Main_Auxiliar.h"

int PSR::N, PSR::maxN, PSR::minN;
MAux *PSR::Aux;
Respostas PSR::OtimizarComAWR;
long double **PSR::CacheDisponibilidade, ***PSR::CacheDistancias;
long double *PSR::Coeficientes, *PSR::ComprimentosNormalizados;
Particula *PSR::PSO_populacao;
long double PSR::MaiorEnlace = -1, PSR::PSO_Vmax = 1, PSR::PSO_Vmin = -1, PSR::PSO_Xmax = 1, PSR::PSO_Xmin = -1;
int PSR::PSO_P, PSR::PSO_G;
long double PSR::PSO_c1, PSR::PSO_c2, PSR::PSO_chi, PSR::PSO_MelhorPbReq = 1;
ifstream PSR::PSO_Coeficientes_R("PSOCoeficientes.txt");
PSR::Custo PSR::C = PSR::DistanciaNumFormas;
PSR::Tipo PSR::T = PSR::Tensorial;

void clearMemory(); /*Limpa e zera todas as constantes de Def.h, reinicia o tempo de simulação e libera todos os slots.*/
void RemoveCon(Event*); /*Retira uma conexão da rede - liberando todos os seus slots*/
void RequestCon(Event*); /*Cria uma conexão. Dados dois nós, procura pelo algoritmo de roteamento definido uma rota entre os mesmos. Após encontrar a rota, cria a conexão, e por fim agenda o próximo evento de requisição de conexão.*/
void setReqEvent(Event*, TIME); /*Cria um evento de requisição a partir do instante de criação (TIME)*/
long double Simula_Rede(Def *Config, MAux *Aux);
void SimAlfaBeta();

PSR::PSR(int Nmin, int Nmax, MAux *A = 0) {
	assert(Nmax > Nmin);
	MaiorEnlace = 0;
	N = Nmax - Nmin + 1;
	maxN = Nmax;
	minN = Nmin;
	Aux = A;

	Coeficientes = new long double[PSR::get_N()*PSR::get_N()];
	ComprimentosNormalizados = new long double[Def::getNnodes()*Def::getNnodes()];
	CacheDistancias = new long double**[PSR::get_N()];
	CacheDisponibilidade = new long double*[Def::getSE()+1];
	for (int i = 0; i < PSR::get_N(); i++) {
		CacheDistancias[i] = new long double*[Def::Nnodes];
		for (int j = 0; j < Def::Nnodes; j++)
			CacheDistancias[i][j] = new long double[Def::Nnodes];
	}
	for (int i = 0; i < Def::getSE()+1; i++)
		CacheDisponibilidade[i] = new long double[PSR::get_N()];
	Normalizacao();
	criarCache();
}

void PSR::criarCache() {
	long double aux;
	for (int i = 0; i < Def::getNnodes(); i++) {
		for (int j = 0; j < Def::getNnodes(); j++) {
			aux = MAux::Caminho[i].at(j).get_comprimento();
			for (int k = minN; k <= maxN; k++) {
				if (MAux::Topology[Def::getNnodes()*i + j] == 0)
					CacheDistancias[k-minN][i][j] = Def::MAX_DOUBLE;
				else
					if ((C == DistanciaDisponibilidade) || (C == DistanciaNumFormas) || (C==DistanciaFormasNormalizado))
						CacheDistancias[k-minN][i][j] = pow( aux/get_MaiorEnlace(), k );
					else if ((C == RuidoDisponibilidade) || (C == RuidoNumFormas) || (C==RuidoFormasNormalizado))
						CacheDistancias[k-minN][i][j] = Def::MAX_DOUBLE;
			}
		}
	}

	for (int i = 0; i <= Def::getSE(); i++) {
		if ((C == DistanciaDisponibilidade) || (C == RuidoDisponibilidade)) aux = (i*1.0)/Def::getSE();
		else if ((C == DistanciaNumFormas) || (C == RuidoNumFormas)) aux = (1.0)/(i + 1);
		else if ((C == DistanciaFormasNormalizado) || (C == RuidoFormasNormalizado)) aux = 1.0/(Def::getSE() - i + 1);
		for (int j = minN; j <= maxN; j++)
			CacheDisponibilidade[i][j-minN] = pow(aux, j);
	}
}

const int PSR::get_N() {
	return PSR::N;
}

const int PSR::get_NMax() {
	return maxN;
}

const int PSR::get_NMin() {
	return minN;
}

long double PSR::get_coeficiente(int i, int j) {
	assert ((i < get_N()) && (i >= 0));
	assert ((j < get_N()) && (j >= 0));
	return Coeficientes[i*PSR::N+j];
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
			if (MAux::Topology[i*Def::Nnodes+j] == 0) {
				ComprimentosNormalizados[i*Def::Nnodes+j] = Def::MAX_LONGDOUBLE;
				continue;
			}
			ComprimentosNormalizados[i*Def::Nnodes+j] = MAux::Caminho[i].at(j).get_comprimento() / MaiorEnlace;
		}
	}
}

void PSR::PSO_configurar() {
	PSO_P = 50;
	PSO_G = 500;
	PSO_c1 = 2.05;
	PSO_c2 = 2.05;

	long double phi = PSO_c1 + PSO_c2;
	PSO_chi = -2.0/(2 - phi - sqrt(phi*phi - 4*phi));

	PSO_populacao = new Particula[PSO_P];
	if (PSR::T == PSR::Matricial)
		for (int i = 0; i < PSO_P; i++) {
			if (i!=0) PSO_populacao[i].Vizinha1 = PSO_populacao + i - 1;
			else PSO_populacao[i].Vizinha1 = PSO_populacao + PSO_P - 1;
			if(i!=PSO_P-1) PSO_populacao[i].Vizinha2 = PSO_populacao + i + 1;
			else PSO_populacao[i].Vizinha2 = PSO_populacao;

			PSO_populacao[i].x = new long double[N*N];
			PSO_populacao[i].v = new long double[N*N];
			PSO_populacao[i].p = new long double[N*N];
		}
	else if (PSR::T == PSR::Tensorial) {
		for (int i = 0; i < PSO_P; i++) {
			if (i!=0) PSO_populacao[i].Vizinha1 = PSO_populacao + i - 1;
			else PSO_populacao[i].Vizinha1 = PSO_populacao + PSO_P - 1;
			if(i!=PSO_P-1) PSO_populacao[i].Vizinha2 = PSO_populacao + i + 1;
			else PSO_populacao[i].Vizinha2 = PSO_populacao;

			PSO_populacao[i].x = new long double[N*N];
			PSO_populacao[i].v = new long double[N*2];
			PSO_populacao[i].p = new long double[N*2];
			PSO_populacao[i].geratriz = new long double[N*2];
		}
	}
}

void PSR::PSO() {
	PSO_configurar();
	PSO_iniciarPopulacao();

	for (int Repeticao = 0; Repeticao < PSO_G; Repeticao++) {
		cout << "PSO - Repeticao " << Repeticao << "." << endl;
		#pragma omp parallel for schedule(dynamic)
		for (int Part = 0; Part < PSO_P; Part++) {
			Def *PSRDef = new Def(PSO_populacao + Part);
			MAux *PSRAux = new MAux();
			long double PbReq = PSO_simulaRede(PSO_populacao + Part, PSRDef, PSRAux);
			if (PbReq < (PSO_populacao + Part)->melhorInd) {
				(PSO_populacao + Part)->melhorInd = PbReq;
				if (PSR::T == PSR::Matricial)
					for (int i = 0; i < N*N; i++)
						(PSO_populacao + Part)->p[i] = (PSO_populacao + Part)->x[i];
				else if (PSR::T == PSR::Tensorial)
					for (int i = 0; i < 2*N; i++)
						(PSO_populacao + Part)->p[i] = (PSO_populacao + Part)->geratriz[i];
			}
			if (PbReq < PSR::PSO_MelhorPbReq) {
				PSR::PSO_MelhorPbReq = PbReq;
				for (int i = 0; i < N; i++)
					for (int j = 0; j < N; j++)
						Coeficientes[i*N + j] = (PSO_populacao+Part)->x[i*N +j];
				PSO_ImprimeCoeficientes();
			}
			delete PSRDef;
			delete PSRAux;
			cout << "Particula " << Part << " PbReq " << PbReq << " (" << PSO_MelhorPbReq << ")" << endl;
		}
			MAux::PSRLog << Repeticao+1 << "\t" << PSO_MelhorPbReq << endl;
		PSO_atualizaVelocidades();
	}
}

void PSR::PSO_iniciarPopulacao() {
	if (PSR::T == PSR::Matricial) {
		for (int i = 0; i < PSO_P; i++) {
			for (int j = 0; j < N; j++) {
				for (int k = 0; k < N; k++) {
					PSO_populacao[i].v[j*N+k] = 0;
					PSO_populacao[i].x[j*N+k] = General::uniforme(0,PSR::PSO_Xmax);
				}
			}
		}

		if (OtimizarComAWR == SIM) {
			assert( PSR::C != DistanciaDisponibilidade );
			assert( PSR::C != RuidoDisponibilidade );
			if (PSR::C == DistanciaNumFormas) MAux::escOtim = OtimizarAlfa;
			else if (PSR::C == RuidoNumFormas) MAux::escOtim = OtimizarBeta;

			if (MAux::escOtim == OtimizarAlfa)
				MAux::Alg_Routing = DJK_SPeFormas;
			else if (MAux::escOtim == OtimizarBeta)
				MAux::Alg_Routing = DJK_RuidoEFormas;

			SimAlfaBeta();

			for (int j = 0; j < N; j++) {
				for (int k = 0; k < N; k++) {
					PSO_populacao[0].x[j*N+k] = 0;
				}
			}

			if (MAux::escOtim == OtimizarAlfa) {
				PSO_populacao[0].x[1] = 0.01*Aux->Config->Alfa;
				PSO_populacao[0].x[N] = 1.0 - 0.01*Aux->Config->Alfa;
			} else {
				PSO_populacao[0].x[1] = 0.01*Aux->Config->Beta;
				PSO_populacao[0].x[N] = 1.0 - 0.01*Aux->Config->Beta;
			}
		}
	} else if (PSR::T == PSR::Tensorial) {
		for (int i = 0; i < PSO_P; i++) {
			for (int k = 0; k < 2*N; k++) {
				PSO_populacao[i].v[k] = 0;
				PSO_populacao[i].geratriz[k] = General::uniforme(0,PSR::PSO_Xmax);
				PSO_gerarPosicao(&PSO_populacao[i]);
			}
		}
	}

	MAux::Alg_Routing = Dij_PSO;
}

void PSR::PSO_atualizaCustoEnlaces(Particula *P) {
	for (int i = 0; i < Def::getNnodes(); i++) {
		for (int j = 0; j < Def::getNnodes(); j++) {
			if (MAux::Topology[i*Def::Nnodes+j] == 1)
				MAux::Caminho[i].at(j).recalcular_peso(P->x);
		}
	}
}

long double PSR::get_MaiorEnlace() {
	if (MaiorEnlace==-1) procurarMaiorEnlace();
	return MaiorEnlace;
}

long double PSR::PSO_simulaRede(Particula *P, Def *Config, MAux *Aux) {
	PSO_atualizaCustoEnlaces(P);
	return Simula_Rede(Config, Aux);
}

void PSR::executar_PSR(MAux *Aux) {
	PSO();
	PSO_ImprimeCoeficientes();
	for (int i = 0; i < PSO_P; i++) {
		delete[] PSO_populacao[i].x;
		delete[] PSO_populacao[i].p;
		delete[] PSO_populacao[i].v;
	}
	for (int i = 0; i < Def::Nnodes*Def::Nnodes; i++) {
		if (!Aux->AllRoutes[i].empty())
			delete Aux->AllRoutes[i].front();
		vector<Route*> ().swap(Aux->AllRoutes[i]);
	}
	delete[] PSO_populacao;
}

void PSR::PSO_atualizaVelocidades() {
	Particula *Fitter;
	long double eps1, eps2;
	for (int i = 0; i < PSO_P; i++) {
		if (PSO_populacao[i].Vizinha1->melhorInd < PSO_populacao[i].Vizinha2->melhorInd)
			Fitter = PSO_populacao[i].Vizinha1;
		else
			Fitter = PSO_populacao[i].Vizinha2;

		if (PSR::T == PSR::Matricial)
			for (int j = 0; j < N; j++) {
				for (int k = 0; k < N; k++) {
					eps1 = General::uniforme(0,1);
					eps2 = General::uniforme(0,1);
					PSO_populacao[i].v[j*N+k] = PSO_chi * ( PSO_populacao[i].v[j*N+k] + PSO_c1*eps1*( PSO_populacao[i].p[j*N+k] - PSO_populacao[i].x[j*N+k] ) + PSO_c2*eps2*( Fitter->p[j*N+k] - PSO_populacao[i].x[j*N+k] ) );
					if (PSO_populacao[i].v[j*N+k] > PSO_Vmax) PSO_populacao[i].v[j*N+k] = PSO_Vmax;
					if (PSO_populacao[i].v[j*N+k] < PSO_Vmin) PSO_populacao[i].v[j*N+k] = PSO_Vmin;
					PSO_populacao[i].x[j*N+k] += PSO_populacao[i].v[j*N+k];
					if (PSO_populacao[i].x[j*N+k] > PSO_Xmax) PSO_populacao[i].x[j*N+k] = PSO_Xmax;
					if (PSO_populacao[i].x[j*N+k] < PSO_Xmin) PSO_populacao[i].x[j*N+k] = PSO_Xmin;
				}
			}
		else if (PSR::T == PSR::Tensorial) {
			for (int j = 0; j < 2*N; j++) {
				eps1 = General::uniforme(0,1);
				eps2 = General::uniforme(0,1);
				PSO_populacao[i].v[j] = PSO_chi * ( PSO_populacao[i].v[j] + PSO_c1*eps1*( PSO_populacao[i].p[j] - PSO_populacao[i].geratriz[j] ) + PSO_c2*eps2*( Fitter->p[j] - PSO_populacao[i].geratriz[j] ) );
				if (PSO_populacao[i].v[j] > PSO_Vmax) PSO_populacao[i].v[j] = PSO_Vmax;
				if (PSO_populacao[i].v[j] < PSO_Vmin) PSO_populacao[i].v[j] = PSO_Vmin;
				PSO_populacao[i].geratriz[j] += PSO_populacao[i].v[j];
				if (PSO_populacao[i].geratriz[j] > PSO_Xmax) PSO_populacao[i].geratriz[j] = PSO_Xmax;
				if (PSO_populacao[i].geratriz[j] < PSO_Xmin) PSO_populacao[i].geratriz[j] = PSO_Xmin;
			}
			PSO_gerarPosicao(&PSO_populacao[i]);
		}
	}
}

void PSR::PSO_ImprimeCoeficientes() {
	ofstream PSO_Coeficientes_W("PSOCoeficientes.txt");
	PSO_Coeficientes_W << PSR::C << endl;
	PSO_Coeficientes_W << PSR::T << endl;
	PSO_Coeficientes_W << minN << "\t" << maxN << endl;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			PSO_Coeficientes_W << Coeficientes[i*PSR::N+j] << "\t";
		}
		PSO_Coeficientes_W << endl;
	}
	PSO_Coeficientes_W << endl << PSO_MelhorPbReq << endl;
}

long double PSR::get_Disponibilidade(int NSlots, int N, int L) {
	assert(N <= PSR::get_NMax());
	assert(N >= PSR::get_NMin());
	assert(NSlots <= Def::getSE());

	if ((C != DistanciaFormasNormalizado) && (C != RuidoFormasNormalizado))
		return CacheDisponibilidade[NSlots][N-minN];
	else
		return pow(NSlots,N)*CacheDisponibilidade[L][N-minN];
}

long double PSR::get_Distancia(int WhoAmI1, int WhoAmI2, int N) {
	assert(WhoAmI1 < Def::getNnodes());
	assert(WhoAmI2 < Def::getNnodes());
	assert(N <= PSR::get_NMax());
	assert(N >= PSR::get_NMin());

	return CacheDistancias[N-minN][WhoAmI1][WhoAmI2];
}

void PSR::PSO_gerarPosicao(Particula *P) {
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			P->x[i*N + j] = P->geratriz[i]*P->geratriz[j+N];
		}
	}
}
