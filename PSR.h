#ifndef PSR_H
#define PSR_H

#include "RWA.h"
#include "assert.h"
#include "Def.h"
#include "Enlace.h"
#include "Route.h"
#include "vector"


class PSR {
	public:
		PSR(int);
		~PSR();
		const static int get_N();
		static long double procurarMaiorEnlace();
		static void executar_PSR();
	private:
		static int N; //ponto de truncamento
		static long double **Coeficientes;
		static long double MaiorEnlace;
		static long double **ComprimentosNormalizados, **DisponibilidadeNormalizada;

		static long double get_coeficiente(int, int);
		static void Normalizacao();
};

#endif // PSR_H
