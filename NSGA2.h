#ifndef NSGA2_H
#define NSGA2_H

class NSGA2 {
	private:
		enum Parametros{
			PbReq, CapEx, OpEx, Energia, ParamMAX
		};
		struct Individuo {
			long double *Gene;
			long double Objetivos[ParamMAX];
			int Aptidao;
		};
		struct Geracao {
			Individuo *Populacao;
			int idGeracao;
		};

		int g; //geracao atual
		static int G; //numero de geracoes
		static int S; //numero de individuos
		static int T; //numero de genes

		static long double Pc; //probabilidade de cruzamento
		static long double Pm; //probabilidade de mutacao

		void criar_GeracaoInicial();
		void criar_Geracao(Geracao *);
		void criar_Populacao(Geracao);
		void criar_Individuo(Individuo);

		void executarNSGA2();

		void evalPareto(Geracao *G);
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
