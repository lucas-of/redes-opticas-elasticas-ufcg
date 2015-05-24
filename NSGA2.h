#ifndef NSGA2_H
#define NSGA2_H

#include <vector>

class NSGA2 {
	private:
		enum Parametros{
			PbReq, CapEx, OpEx, Energia, ParamMAX
		};
		struct Individuo {
			int id;
			long double *Gene;
			long double Objetivos[ParamMAX];
			int Aptidao;
			long double crowdDistance;
		};
		struct Geracao {
			std::vector <Individuo*> Populacao;
			int idGeracao;
			int nFrentesPareto;
		};

		int g; //geracao atual
		int k; //parametro para o torneio binario
		static int G; //numero de geracoes
		static int S; //numero de individuos
		static int T; //numero de genes

		static long double Pc; //probabilidade de cruzamento
		static long double Pm; //probabilidade de mutacao

		void criar_GeracaoInicial();
		void criar_Geracao(Geracao *);
		void criar_Populacao(Geracao);
		void criar_Individuo(Individuo);

		void Crumento(int I1, int I2, Geracao *);
		void Mutacao(int I1, Geracao *);
		Geracao* Selecao(Geracao *);
		Individuo* TorneioBinario(Geracao *);

		void executarNSGA2();

		void evalPareto(Geracao *G);
		void evalcrowdDistance(Geracao *G);
		bool A_Domina_B(Individuo *A, Individuo *B);

		void evalFuncoesCusto(Individuo *);
		long double evalPbReq(Individuo*);
		long double evalCapEx(Individuo*);
		long double evalOpEx(Individuo*);
		long double evalEnergia(Individuo *);
	public:
		NSGA2();
		~NSGA2();
		static Geracao *Evolucao;
};

#endif // NSGA2_H
