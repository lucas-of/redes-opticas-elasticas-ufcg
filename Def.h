#ifndef Def_H
#define Def_H

#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

enum EsquemaDeModulacao { _4QAM = 4, _16QAM = 16, _64QAM = 64 };
const int numEsquemasDeModulacao = 3;

using namespace std;

typedef long double TIME;

#include "General.h"

class Def {
    public:
        enum Arquitetura {
            BS, SS
        }; /* Broadcast-and-Select ou Switch-and-Select*/
        static long double PossiveisTaxas[];
        const static int numPossiveisTaxas;
        static int Nnodes; // N é o número de nós;

    private:
        static vector<int> GrauNo; //O grau de cada no
        static vector<double> LaNet; //La é o tráfego por cada slot
        static long double numReqBloqMin; //é o número máximo de requisições bloqueadas
        static int SE; //S é o número de slots por Enlace
        static int SR; //SR é o número máximo de slots requisitados

        static long double Pin;//Pin potencia de entrada
        static long double Pref;//Pref potencia de referencia
        static long double OSNRin;//OSNRin OSNR de entrada
        static long double Lsss;//Lsssm perdas nos dispositivos
        static long double DistA;//DistA distancia entre os amplificadores de linha
        static Arquitetura arquitetura;//arquitetura de nó sendo 1=BS e 2=SS
        static long double lambda;//comprimento de onda (em metros)
        static long double Bslot;//largura de banda do slot
        static long double Bref;//largura de linha de referencia
        static long double Famp;//fator do ruido amplificador
        static long double Dcr; //relacionado a dispersao cromatica
        static long double DDCF; //coeficiente de dispersao
        static long double freq; //frequencia
        static long double get_snrb(EsquemaDeModulacao); /*retorna a SNR de qualidade, por bit*/

    public:
        static float Alfa; //SPeFormas
        static float Beta; //RuidoNormalizadoeFormas
        static double MAX_DOUBLE; //limites - maior DOUBLE possível
        static int MAX_INT; //limites - maior INT possível
        static double MAX_LONGDOUBLE;  //limites - maior LONG DOUBLE possível
        static long double netOccupancy; //ocupação da rede
        static long double numHopsPerRoute; //número de nós por rota
        static long double numReq; //número de requisições
        static long double numReq_Bloq; //número de requisições bloqueadas
        static long double numReq_BloqPorOSNR; //número de requisições bloqueadas por OSNR
        static const int numEsquemasDeModulacao;
        static long double numSlots_Bloq; //número de slots bloqueados
        static long double numSlots_Req; //número de slots requisitados
        static long double numSlots_BloqPorOSNR; //número de slots bloqueados por OSNR
        static long double numReq_Taxa[];
        static long double numReqBloq_Taxa[];
        static long double numReqAceit_Esquema[];
        static long double tempoTotal_Taxa[];
        static long double taxaTotal_Esquema[];
        static long double taxaTotal;

        static void clearGrauNo();
        static void setPossiveisTaxas();
        static long double get_Bslot();
        static long double get_Bref();
        static int getGrauNo(int);
        static long double getlambda(void);
        static double getLaNet(int);
        static double getlimiarOSNR(EsquemaDeModulacao, long double);
        static int getNnodes();
        static long double getNumReqBloqMin();
        static void setNnodes(int);
        static int getSE();
        static int getSR();
        static void set_Arquitetura(Arquitetura);
        static void set_DistaA(long double);
        static void set_Lsss(long double);
        static void set_Pin(long double);
        static void set_Pref(long double);

        static void setBslot(double);
        static void setBref(double);
        static void setGrauNo(int Grau);
        static void setLaCheck(double); //confirma que a soma do tráfego é normalizado
        static void setLaRandom(double); //tráfego aleatório entre slots
        static void setLaUniform(double); //tráfego uniforme entre enlaces
        static void setNumReqBloqMin(long double);
        static void setSE(int);
        static void setSR(int);
        static void setOSNR(long double);

        static Arquitetura get_Arquitetura();
        static long double get_Pin();
        static long double get_Pref();
        static long double get_OSRNin();
        static long double get_Lsss();
        static long double get_DistaA();
        static long double get_Famp();
        static long double get_freq();
        static long double get_DDCF();
        static long double get_Dcr();
        static int get_numPossiveisTaxas();
};
#endif
