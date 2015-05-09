#ifndef PSR_H
#define PSR_H

#include "RWA.h"
#include "assert.h"
#include "Def.h"
#include "Enlace.h"
#include "Route.h"
#include "General.h"
#include "vector"

class PSR{
	public:
		PSR(int);
		const static int get_N();
		static void executar_PSR(MAux *Aux);
		static long double get_MaiorEnlace();
		static void criarCache();
		static long double get_Disponibilidade(int NSlots, int N);
		static long double get_Distancia(int WhoAmI1, int WhoAmI2, int N);
		static long double *ComprimentosNormalizados;
		static ifstream PSO_Coeficientes_R;

	private:
		static long double *CacheDistancias;
		static long double *CacheDisponibilidade;
		static int N; //ponto de truncamento
		static long double *Coeficientes;
		static long double MaiorEnlace;

		static int PSO_P; //numero de partículas
		static int PSO_G; //numero de iterações
		static long double PSO_MelhorPbReq;
		static long double PSO_c1;
		static long double PSO_c2; //parâmetros das velocidades
		static long double PSO_chi; //fator de constrição
		static long double PSO_Vmax;
		static long double PSO_Vmin; //velocidade maxima e minima
		static long double PSO_Xmax;
		static long double PSO_Xmin; //posicao maxima e minima
		static Particula *PSO_populacao;
		static ofstream PSO_Coeficientes_W;

		static long double get_coeficiente(int, int);
		static void Normalizacao();
		static long double procurarMaiorEnlace();

		static void PSO();
		static void PSO_configurar();
		static void PSO_iniciarPopulacao();
		static void PSO_atualizaCustoEnlaces(Particula *P);
		static long double PSO_simulaRede(Particula *P, Def *Config, MAux *Aux);
		static void PSO_atualizaVelocidades();
		static void PSO_ImprimeCoeficientes();
};

#endif // PSR_H
