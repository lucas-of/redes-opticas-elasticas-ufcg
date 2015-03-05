#ifndef PSR_H
#define PSR_H

#include "RWA.h"
#include "assert.h"
#include "Def.h"
#include "Enlace.h"
#include "Route.h"
#include "General.h"
#include "vector"

struct Particula {
		long double **x;
		long double **v;
		long double **p;
		long double melhorInd = 1;
};

class PSR {
	public:
		PSR(int);
		~PSR();
		const static int get_N();
		static void executar_PSR();
		static long double get_MaiorEnlace();
		static long double **ComprimentosNormalizados;
	private:
		static int N; //ponto de truncamento
		static long double **Coeficientes;
		static long double MaiorEnlace;

		static int PSO_P; //numero de partículas
		static int PSO_G; //numero de iterações
		static long double PSO_MelhorPbReq;
		static long double PSO_c1;
		static long double PSO_c2; //parâmetros das velocidades
		static long double PSO_chi; //fator de constrição
		static Particula *PSO_populacao, *Melhor;

		static long double get_coeficiente(int, int);
		static void Normalizacao();
		static long double procurarMaiorEnlace();

		static void PSO();
		static void PSO_configurar();
		static void PSO_iniciarPopulacao();
		static void PSO_atualizaCustoEnlaces(Particula *P);
		static long double PSO_simulaRede(Particula *P);
		static void PSO_atualizaVelocidades();
};

#endif // PSR_H
