#ifndef NSGA2_H
#define NSGA2_H

class NSGA2 {
	private:
		struct Individuo {
			long double *Gene;
		};
		struct Geracao {
			Individuo *Populacao;
		};

		static int G; //numero de geracoes
		static int S; //numero de individuos
		static int T; //numero de genes
		static int RiMax; //numero maximo de regeneradores

		void criar_GeracaoInicial();
		void criar_Geracao(int numGeracao);
		void criar_Populacao(Geracao);
		void criar_Individuo(Individuo);
	public:
		NSGA2(int nG);
		~NSGA2();
		static Geracao *Evolucao;
};

#endif // NSGA2_H
