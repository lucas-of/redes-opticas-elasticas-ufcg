#ifndef NSGA2_H
#define NSGA2_H

class NSGA2 {
	private:
		struct Individuo {
			long double *Gene;
			long double PbReq, CapEx, NNT;
		};
		struct Geracao {
			Individuo *Populacao;
			int idGeracao;
		};

		int g; //geracao atual
		static int G; //numero de geracoes
		static int S; //numero de individuos
		static int T; //numero de genes

		void criar_GeracaoInicial();
		void criar_Geracao(Geracao *);
		void criar_Populacao(Geracao);
		void criar_Individuo(Individuo);

		void executarNSGA2();

		void evalFuncoesCusto(Individuo *);
		long double evalCapEx(Individuo*);
		long double evalPbReq(Individuo*);
		long double evalNNT(Individuo*);
	public:
		NSGA2();
		~NSGA2();
		static Geracao *Evolucao;
};

#endif // NSGA2_H
