#include "PSR.h"
#include "Main_Auxiliar.h"
#include <QtConcurrent>

int PSR::N;
long double *PSR::Coeficientes, *PSR::ComprimentosNormalizados;
Particula *PSR::PSO_populacao;
long double PSR::MaiorEnlace = -1, PSR::PSO_Vmax = 1, PSR::PSO_Vmin = -1, PSR::PSO_Xmax = 1, PSR::PSO_Xmin = 0;
int PSR::PSO_P, PSR::PSO_G;
long double PSR::PSO_c1, PSR::PSO_c2, PSR::PSO_chi, PSR::PSO_MelhorPbReq = 1;
ifstream PSR::PSO_Coeficientes_R("PSOCoeficientes.txt");

void clearMemory(); /*Limpa e zera todas as constantes de Def.h, reinicia o tempo de simulação e libera todos os slots.*/
void RemoveCon(Event*); /*Retira uma conexão da rede - liberando todos os seus slots*/
void RequestCon(Event*); /*Cria uma conexão. Dados dois nós, procura pelo algoritmo de roteamento definido uma rota entre os mesmos. Após encontrar a rota, cria a conexão, e por fim agenda o próximo evento de requisição de conexão.*/
void setReqEvent(Event*, TIME); /*Cria um evento de requisição a partir do instante de criação (TIME)*/
long double Simula_Rede();

PSR::PSR(int NewN) {
    assert(NewN > 0);
    MaiorEnlace = 0;
    N = NewN;

    Coeficientes = new long double[PSR::get_N()*PSR::get_N()];
    ComprimentosNormalizados = new long double[Def::getNnodes()*Def::getNnodes()];
    Normalizacao();
}

const int PSR::get_N() {
    return PSR::N;
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
    for (int i = 0; i < PSO_P; i++) {
        if (i!=0) PSO_populacao[i].Vizinha1 = PSO_populacao + i - 1;
        else PSO_populacao[i].Vizinha1 = PSO_populacao + PSO_P - 1;
        if(i!=PSO_P-1) PSO_populacao[i].Vizinha2 = PSO_populacao + i + 1;
        else PSO_populacao[i].Vizinha2 = PSO_populacao;

        PSO_populacao[i].x = new long double[N*N];
        PSO_populacao[i].v = new long double[N*N];
        PSO_populacao[i].p = new long double[N*N];
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
                for (int i = 0; i < N*N; i++)
                (PSO_populacao + Part)->p[i] = (PSO_populacao + Part)->x[i];
            }
            if (PbReq < PSR::PSO_MelhorPbReq) {
                PSR::PSO_MelhorPbReq = PbReq;
                for (int i = 0; i < N; i++)
                    for (int j = 0; j < N; j++)
                        Coeficientes[i*N + j] = (PSO_populacao+Part)->x[i*N +j];
                PSO_ImprimeCoeficientes();
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
                PSO_populacao[i].v[j*N+k] = 0;
                PSO_populacao[i].x[j*N+k] = General::uniforme(PSR::PSO_Xmin,PSR::PSO_Xmax);
            }
        }
    }
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

long double PSR::PSO_simulaRede(Particula *P) {
    PSO_atualizaCustoEnlaces(P);
    return QtConcurrent::run(Simula_Rede);
}

void PSR::executar_PSR() {
    PSO();
    PSO_ImprimeCoeficientes();
    for (int i = 0; i < PSO_P; i++) {
        delete[] PSO_populacao[i].x;
        delete[] PSO_populacao[i].p;
        delete[] PSO_populacao[i].v;
    }
    for (int i = 0; i < Def::Nnodes*Def::Nnodes; i++) {
        if (!MAux::AllRoutes[i].empty())
            delete MAux::AllRoutes[i].front();
        vector<Route*> ().swap(MAux::AllRoutes[i]);
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
    }
}

void PSR::PSO_ImprimeCoeficientes() {
    ofstream PSO_Coeficientes_W("PSOCoeficientes.txt");
    PSO_Coeficientes_W << N << endl;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            PSO_Coeficientes_W << Coeficientes[i*PSR::N+j] << "\t";
        }
        PSO_Coeficientes_W << endl;
    }
    PSO_Coeficientes_W << endl << PSO_MelhorPbReq << endl;
}
