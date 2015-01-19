﻿#ifndef Def_H
#define Def_H

#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

typedef long double TIME;

#include "General.h"

class Def {
    public:
        enum Arquitetura {
            BS, SS
        }; /* Broadcast-and-Select ou Switch-and-Select*/
        enum Compressao {
            COMP1 = 1, COMP2 = 2, COMP4 = 4
        }; /*depende do esquema de modulacao usado */
        static long double PossiveisTaxas[];
        const static int numPossiveisTaxas;

    private:
        static vector<int> GrauNo; //O grau de cada no
        static vector<double> LaNet; //La é o tráfego por cada slot
        static int Nnodes; // N é o número de nós;
        static long double numReqMax; //é o número máximo de requisições
        static int SE; //S é o número de slots por Enlace
        static int SR; //SR é o número máximo de slots requisitados

        static long double Pin;//Pin potencia de entrada
        static long double OSNRin;//OSNRin OSNR de entrada
        static long double Lsss;//Lsssm perdas nos dispositivos
        static long double DistA;//DistA distancia entre os amplificadores de linha
        static Arquitetura arquitetura;//arquitetura de nó sendo 1=BS e 2=SS
        static Compressao compressao;//compressão devido a esquema de modulação
        static long double lambda;//comprimento de onda (em metros)
        static long double Bslot;//largura de banda do slot
        static long double Famp;//fator do ruido amplificador
        static long double Dcr; //relacionado a dispersao cromatica
        static long double DDCF; //coeficiente de dispersao

    public:
        static long double limiarOSNR; //limiar de OSNR para estabelecimento de conexão
        static double MAX_DOUBLE; //limites - maior DOUBLE possível
        static int MAX_INT; //limites - maior INT possível
        static double MAX_LONGDOUBLE;  //limites - maior LONG DOUBLE possível
        static long double netOccupancy; //ocupação da rede
        static long double numHopsPerRoute; //número de nós por rota
        static long double numReq; //número de requisições
        static long double numReq_Bloq; //número de requisições bloqueadas
        static long double numReq_BloqPorOSNR; //número de requisições bloqueadas por OSNR
        static long double numSlots_Bloq; //número de slots bloqueados
        static long double numSlots_Req; //número de slots requisitados
        static long double numSlots_BloqPorOSNR; //número de slots bloqueados por OSNR
        static long double numReq_Taxa[];
        static long double numReqBloq_Taxa[];
        static long double tempoTotal_Taxa[];

        static void clearGrauNo();
        static long double get_Bslot();
        static int getGrauNo(int);
        static long double getlambda(void);
        static double getLaNet(int);
        static double getlimiarOSNR();
        static int getNnodes();
        static long double getNumReqMax();
        static void setNnodes(int);
        static int getSE();
        static int getSR();
        static void set_Arquitetura(Arquitetura);
        static void set_DistaA(long double);
        static void set_Lsss(long double);
        static void set_Pin(long double);

        static void setBslot(double);
        static void setGrauNo(int Grau);
        static void setLaCheck(double); //confirma que a soma do tráfego é normalizado
        static void setLaRandom(double); //tráfego aleatório entre slots
        static void setLaUniform(double); //tráfego uniforme entre enlaces
        static void setLimiarOSNR(double);
        static void setNumReqMax(long double);
        static void setCompressao(Compressao);
        static void setSE(int);
        static void setSR(int);
        static void setOSNR(long double);

        static Arquitetura get_Arquitetura();
        static int get_Compressao();
        static long double get_Pin();
        static long double get_OSRNin();
        static long double get_Lsss();
        static long double get_DistaA();
        static long double get_Famp();
        static long double get_DDCF();
        static long double get_Dcr();
        static int get_numPossiveisTaxas();
};
#endif
