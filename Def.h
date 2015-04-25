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
        Def();
		enum Arquitetura {
			BS, SS
		}; /* Broadcast-and-Select ou Switch-and-Select*/
		static long double PossiveisTaxas[];
		const static int numPossiveisTaxas;
		static int Nnodes; // N é o número de nós;

	private:
		static vector<int> GrauNo; //O grau de cada no
		vector<double> LaNet; //La é o tráfego por cada slot
		static long double numReqBloqMin; //é o número minimo de requisições bloqueadas
		static long double numReqMax; //numero maximo de requisições
		static int SE; //S é o número de slots por Enlace
		static int SR; //SR é o número máximo de slots requisitados

		static long double Pin;//Pin potencia de entrada
		static long double Pref;//Pref potencia de referencia
		long double OSNRin;//OSNRin OSNR de entrada
		static long double Lsss;//Lsssm perdas nos dispositivos
		long double DistA;//DistA distancia entre os amplificadores de linha
		static Arquitetura arquitetura;//arquitetura de nó sendo 1=BS e 2=SS
		static long double lambda;//comprimento de onda (em metros)
		static long double Bslot;//largura de banda do slot
		static long double Bref;//largura de linha de referencia
		static long double Famp;//fator do ruido amplificador
		static long double freq; //frequencia
        static long double get_snrb(EsquemaDeModulacao); /*retorna a SNR de qualidade, por bit*/

	public:
		static float Alfa; //SPeFormas
		static float Beta; //RuidoNormalizadoeFormas
		static double MAX_DOUBLE; //limites - maior DOUBLE possível
		static int MAX_INT; //limites - maior INT possível
		static double MAX_LONGDOUBLE;  //limites - maior LONG DOUBLE possível
		long double netOccupancy; //ocupação da rede
		long double numHopsPerRoute; //número de nós por rota
		long double numReq; //número de requisições
		long double numReq_Bloq; //número de requisições bloqueadas
		long double numReq_BloqPorOSNR; //número de requisições bloqueadas por OSNR
		static const int numEsquemasDeModulacao;
		long double numSlots_Bloq; //número de slots bloqueados
		long double numSlots_Req; //número de slots requisitados
		long double numSlots_BloqPorOSNR; //número de slots bloqueados por OSNR
		long double *numReq_Taxa;
		long double *numReqBloq_Taxa;
		long double *numReqAceit_Esquema;
		long double *tempoTotal_Taxa;
		long double *taxaTotal_Esquema;
		long double taxaTotal;

		void clearGrauNo();
		static void setPossiveisTaxas();
		static long double get_Bslot();
		static long double get_Bref();
		static int getGrauNo(int);
		static long double getlambda(void);
		double getLaNet(int);
		static double getlimiarOSNR(EsquemaDeModulacao, long double);
		static int getNnodes();
		static long double getNumReqBloqMin();
		static long double getNumReqMax();
		static void setNnodes(int);
		static int getSE();
		static int getSR();
		static void set_Arquitetura(Arquitetura);
		void set_DistaA(long double);
		void set_Lsss(long double);
		void set_Pin(long double);
		void set_Pref(long double);

		static void setBslot(double);
		static void setBref(double);
		static void setGrauNo(int Grau);
		void setLaCheck(double); //confirma que a soma do tráfego é normalizado
		void setLaRandom(double); //tráfego aleatório entre slots
		void setLaUniform(double); //tráfego uniforme entre enlaces
		static void setNumReqBloqMin(long double);
		static void setSE(int);
		static void setSR(int);
		void setOSNR(long double);

		static Arquitetura get_Arquitetura();
		long double get_Pin();
		long double get_Pref();
		long double get_OSRNin();
		static long double get_Lsss();
		long double get_DistaA();
		static long double get_Famp();
		static long double get_freq();
		static long double get_DDCF();
		static long double get_Dcr();
		static int get_numPossiveisTaxas();
};
#endif
