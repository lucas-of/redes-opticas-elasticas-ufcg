#include "RWA.h"
#include "Main_Auxiliar.h"

bool RWA::CheckSlotAvailability(const Route* route, const int s) {
	assert(s >=0 && s < Def::getSE());
	int L_or, L_de;
	for(unsigned int c = 0; c < route->getNhops(); c++) {
		L_or = route->getNode(c);
		L_de = route->getNode(c+1);
		if(MAux::Topology_S[s][L_or][L_de] == true)
			return false; //Basta o slot s nao estar disponivel em uma das fibras da rota;
	}
	return true; //O slot s esta disponivel em todas as fibras da rota;
}

void RWA::Dijkstra() {
	int orN, deN, VA, i, j, k=0, path, h, hops;
	long double min;
	vector<Node*> r;
	long double *CustoVertice = new long double[Def::getNnodes()];
	int *Precedente = new int[Def::getNnodes()];
	int *PathRev = new int[Def::getNnodes()];
	bool *Status = new bool[Def::getNnodes()];

	//Busca para todos os pares de no a rota mais curta:
	for(orN = 0; orN < Def::getNnodes(); orN++) {
		for(i = 0; i < Def::getNnodes(); i++) {
			if(i != orN)
				CustoVertice[i] = Def::MAX_DOUBLE;
			else
				CustoVertice[i] = 0.0;
			Precedente[i] = -1;
			Status[i] = 0;
		}
		VA = Def::getNnodes();
		while(VA > 0) {
			min = Def::MAX_DOUBLE;
			for(i = 0; i < Def::getNnodes(); i++)
				if((Status[i] == 0)&&(CustoVertice[i] < min)) {
					min = CustoVertice[i];
					k = i;
				}
			Status[k] = 1;
			VA = VA-1;
			for(j = 0; j < Def::getNnodes(); j++)
				if((Status[j] == 0)&&(MAux::Topology[k][j] != 0)&&(CustoVertice[k]+MAux::Topology[k][j] < CustoVertice[j])) {
					CustoVertice[j] = CustoVertice[k]+MAux::Topology[k][j];
					Precedente[j] = k;
				}
		}

		for(deN = 0; deN < Def::getNnodes(); deN++) {
			path = orN*Def::getNnodes()+deN;
			MAux::AllRoutes[path].clear();
			if(deN != orN) {
				PathRev[0] = deN;
				hops = 0;
				j = deN;
				while(j != orN) {
					hops = hops+1;
					PathRev[hops] = Precedente[j];
					j = Precedente[j];
				}
				r.clear();
				for(h = 0; h <= hops; h++)
					r.push_back(&MAux::Rede.at(PathRev[hops-h]));
				MAux::AllRoutes[path].push_back(new Route(r));
			}
		}
	}

	//Imprimir Rotas:
	for(orN = 0; orN < Def::getNnodes(); orN++)
		for(deN = 0; deN < Def::getNnodes(); deN++)
			if(orN != deN) {
				cout << endl << "[orN="<<orN<<"  deN="<<deN<<"]  route = ";
				path = orN*Def::getNnodes()+deN;
				hops = MAux::AllRoutes[path].at(0)->getNhops();
				cout << hops << " hops: ";
				if(hops != 0)
					for(h = 0; h <= hops; h++)
						cout<<MAux::AllRoutes[path].at(0)->getNode(h)<<"-";
			}
	cout<<endl<<endl;
	delete []CustoVertice;
	delete []Precedente;
	delete []Status;
	delete []PathRev;
}

void RWA::DijkstraSP() {
	int orN, deN, VA, i, j, k=0, path, h, hops;
	long double min;
	vector<Node*> r;
	long double *CustoVertice = new long double[Def::getNnodes()];
	int *Precedente = new int[Def::getNnodes()];
	int *PathRev = new int[Def::getNnodes()];
	bool *Status = new bool[Def::getNnodes()];

	MAux::MinimasDistancias = new long double*[Def::getNnodes()];
	for (i = 0; i < Def::getNnodes(); i++) MAux::MinimasDistancias[i] = new long double[Def::getNnodes()];
	cout << Def::getNnodes() << endl;
	//Busca para todos os pares de no a rota mais curta:
	for(orN = 0; orN < Def::getNnodes(); orN++) {
		for(i = 0; i < Def::getNnodes(); i++) {
			if(i != orN)
				CustoVertice[i] = Def::MAX_DOUBLE;
			else
				CustoVertice[i] = 0.0;
			Precedente[i] = -1;
			Status[i] = 0;
		}
		VA = Def::getNnodes();
		while(VA > 0) {
			min = Def::MAX_DOUBLE;
			for(i = 0; i < Def::getNnodes(); i++)
				if((Status[i] == 0)&&(CustoVertice[i] < min)) {
					min = CustoVertice[i];
					k = i;
				}
			Status[k] = 1;
			VA = VA-1;
			for(j = 0; j < Def::getNnodes(); j++)
				if((Status[j] == 0)&&(MAux::Topology[k][j] != 0)&&(CustoVertice[k]+MAux::Caminho[j].at(k).get_comprimento() < CustoVertice[j])) {
					CustoVertice[j] = CustoVertice[k]+MAux::Caminho[j].at(k).get_comprimento();
					Precedente[j] = k;
				}
		}

		for (j = 0; j< Def::getNnodes(); j++) MAux::MinimasDistancias[orN][j] = CustoVertice[j];

		for(deN = 0; deN < Def::getNnodes(); deN++) {
			path = orN*Def::getNnodes()+deN;
			MAux::AllRoutes[path].clear();
			if(deN != orN) {
				PathRev[0] = deN;
				hops = 0;
				j = deN;
				while(j != orN) {
					hops = hops+1;
					PathRev[hops] = Precedente[j];
					j = Precedente[j];
				}
				r.clear();
				for(h = 0; h <= hops; h++)
					r.push_back(&MAux::Rede.at(PathRev[hops-h]));
				MAux::AllRoutes[path].push_back(new Route(r));
			}
		}
	}

	//Imprimir Rotas:
	for(orN = 0; orN < Def::getNnodes(); orN++)
		for(deN = 0; deN < Def::getNnodes(); deN++)
			if(orN != deN) {
				cout << endl << "[orN="<<orN<<"  deN="<<deN<<"]  route = ";
				path = orN*Def::getNnodes()+deN;
				hops = MAux::AllRoutes[path].at(0)->getNhops();
				cout << hops << " hops: ";
				if(hops != 0)
					for(h = 0; h <= hops; h++)
						cout<<MAux::AllRoutes[path].at(0)->getNode(h)<<"-";
					cout << " (" << MAux::MinimasDistancias[orN][deN] << "km)";
			}
	cout<<endl<<endl;
	delete []CustoVertice;
	delete []Precedente;
	delete []Status;
	delete []PathRev;
}

void RWA::DijkstraAcum(const int orN, const int deN, const int L) {
	//L e a largura de banda (em numero de slots) da requisicao
	assert(deN != orN);
	int VA, i, j, s, k=0, h, path, hops, hopsAux;
	long double min, custoAux;
	vector<Node*> r;
	long double *CustoVertice = new long double[Def::getNnodes()];
	bool **DispVertice = new bool*[Def::getNnodes()];
	for (int i=0 ; i<Def::getNnodes() ; i++) DispVertice[i] = new bool[Def::getSE()];
	int *HopsVertice = new int[Def::getNnodes()]; //O numero de hops ate chegar aquele no pelo caminho mais curto
	bool *DispAux = new bool[Def::getSE()];
	int *Precedente = new int[Def::getNnodes()]; //Informa em cada no por onde chegou a rota mais curta
	int *PathRev = new int[Def::getNnodes()];
	bool *Status = new bool[Def::getNnodes()]; //Informa os nos marcados e nao marcados
	//Busca a rota mais curta para os nos orN e deN;
	//Inicializacao do algoritmo:
	for(i = 0; i < Def::getNnodes(); i++) {
		if(i != orN) {
			CustoVertice[i] = Def::MAX_DOUBLE;
			HopsVertice[i] = Def::MAX_INT;
			for(s = 0; s < Def::getSE(); s++)
				DispVertice[i][s] = false; //O slot e assumido indisponivel
		} else {
			CustoVertice[i] = 0.0;
			HopsVertice[i] = 0;
			for(s = 0; s < Def::getSE(); s++)
				DispVertice[i][s] = true; //O slot e assumido disponivel
		}
		Precedente[i] = -1;
		Status[i] = 0; //No ainda nao foi marcado
	}
	//Processo de Dijkstra:
	VA = Def::getNnodes();
	while(VA > 0) {
		//Toma o no de menor custo ainda nao marcado
		min = Def::MAX_DOUBLE;
		for(i = 0; i < Def::getNnodes(); i++)
			if((Status[i] == false)&&(CustoVertice[i] < min)) {
				min = CustoVertice[i];
				k = i; //No k e o de menor custo
			}
		Status[k] = true; //No k passa a ser marcado
		//A partir do no k, atualiza ou nao o custo de seus nos vizinhos (j)
		VA = VA-1;
		for(j = 0; j < Def::getNnodes(); j++)
			if((Status[j] == false)&&(MAux::Topology[k][j] != 0)) {
				//O no j e nao marcado e vizinho do no k
				//Calcula O vetor de disponibilidade em j por uma rota proveniente de k
				for(s = 0; s < Def::getSE(); s++)
					DispAux[s] = DispVertice[k][s] * !MAux::Topology_S[s][k][j];
				//Calcula o numero de hops em j por uma rota proveniente de k
				hopsAux = HopsVertice[k]+1;
				//Checa se o custo em j deve ser atualizado
				custoAux = Heuristics::calculateCostRouting(hopsAux, DispAux, L);
				if(custoAux < CustoVertice[j]) {
					HopsVertice[j] = hopsAux;
					CustoVertice[j] = custoAux;
					for(int s = 0; s < Def::getSE(); s++)
						DispVertice[j][s] = DispAux[s];
					Precedente[j] = k;
				}
			}
	}
	//Descrever a rota selecionada:
	PathRev[0] = deN;
	hops = 0;
	j = deN;
	while(j != orN) {
		hops = hops+1;
		PathRev[hops] = Precedente[j];
		j = Precedente[j];
	}
	r.clear();
	for(h = 0; h <= hops; h++)
		r.push_back(&MAux::Rede.at(PathRev[hops-h]));
	delete []CustoVertice;
	delete []Precedente;
	delete []Status;
	delete []PathRev;
	delete []DispVertice;
	delete []DispAux;
	delete []HopsVertice;

	//Insere a rota nova em AllRoutes
	path = orN*Def::getNnodes()+deN;
	MAux::AllRoutes[path].clear();
	MAux::AllRoutes[path].push_back(new Route(r));
}

void RWA::DijkstraFormas(const int orN, const int deN, const int L) {
	//L e a largura de banda (em numero de slots) da requisicao
	assert(orN != deN);
	int VA, i, j, k=0, path, h, hops;
	long double min;
	bool *DispLink = new bool[Def::getSE()];
	long double *CustoVertice = new long double[Def::getNnodes()];
	long double custoLink;
	int *Precedente = new int[Def::getNnodes()];
	int *PathRev = new int[Def::getNnodes()];
	bool *Status = new bool[Def::getNnodes()];
	//Busca para todos os pares de no a rota mais curta:
	for(i = 0; i < Def::getNnodes(); i++) {
		if(i != orN)
			CustoVertice[i] = Def::MAX_DOUBLE;
		else
			CustoVertice[i] = 0.0;
		Precedente[i] = -1;
		Status[i] = 0;
	}
	VA = Def::getNnodes();
	while(VA > 0) {
		//Procura o vertice de menor custo
		min = Def::MAX_DOUBLE;
		for(i = 0; i < Def::getNnodes(); i++)
			if((Status[i] == 0)&&(CustoVertice[i] < min)) {
				min = CustoVertice[i];
				k = i;
			}
		Status[k] = 1; //k e o vertice de menor custo;
		VA = VA-1;
		//Verifica se precisa atualizar ou nao os vizinhos de k
		for(j = 0; j < Def::getNnodes(); j++)
			if((Status[j] == 0)&&(MAux::Topology[k][j] != 0)) {
				//O no j e nao marcado e vizinho do no k
				//Calcula O vetor de disponibilidade do enlace entre k e j
				for(int s = 0; s < Def::getSE(); s++)
					DispLink[s] = !MAux::Topology_S[s][k][j];
				custoLink = Heuristics::calculateCostLink(DispLink, L);
				if(CustoVertice[k] + custoLink < CustoVertice[j]) {
					CustoVertice[j] = CustoVertice[k] + custoLink;
					Precedente[j] = k;
				}
			}
	}

	//Formar a rota:
	path = orN*Def::getNnodes()+deN;
	MAux::AllRoutes[path].clear();
	PathRev[0] = deN;
	hops = 0;
	j = deN;
	while(j != orN) {
		hops = hops+1;
		PathRev[hops] = Precedente[j];
		j = Precedente[j];
	}
	vector<Node*> r;
	r.clear();
	for(h = 0; h <= hops; h++)
		r.push_back(&MAux::Rede.at(PathRev[hops-h]));
	assert(r.at(0)->get_whoami() == orN && r.at(hops)->get_whoami() == deN);
	MAux::AllRoutes[path].push_back(new Route(r));

	delete []CustoVertice;
	delete []Precedente;
	delete []Status;
	delete []PathRev;
	delete []DispLink;
}

void RWA::DijkstraSPeFormas(const int orN, const int deN, const int L) {
	//L e a largura de banda (em numero de slots) da requisicao
	assert(orN != deN);
	int VA, i, j, k=0, path, h, hops;
	long double min;
	bool *DispLink = new bool[Def::getSE()];
	long double *CustoVertice = new long double[Def::getNnodes()];
	long double custoLink;
	int *Precedente = new int[Def::getNnodes()];
	int *PathRev = new int[Def::getNnodes()];
	bool *Status = new bool[Def::getNnodes()];
	long double MaiorComprimentoEnlace = 0;
	long double alpha = 0.5;
	//Busca para todos os pares de no a rota mais curta:
	for(i = 0; i < Def::getNnodes(); i++) {
		if(i != orN)
			CustoVertice[i] = Def::MAX_DOUBLE;
		else
			CustoVertice[i] = 0.0;
		Precedente[i] = -1;
		Status[i] = 0;
	}
	//Busca Maior Enlace
	for (i = 0; i < Def::getNnodes(); i++) {
		for (int j = i+1; j < Def::getNnodes(); j++) {
			if (MAux::Topology[i][j] == 0) continue;
			if (MAux::Caminho[i].at(j).get_comprimento() > MaiorComprimentoEnlace)
				MaiorComprimentoEnlace = MAux::Caminho[i].at(j).get_comprimento();
		}
	}
	VA = Def::getNnodes();
	while(VA > 0) {
		//Procura o vertice de menor custo
		min = Def::MAX_DOUBLE;
		for(i = 0; i < Def::getNnodes(); i++)
			if((Status[i] == 0)&&(CustoVertice[i] < min)) {
				min = CustoVertice[i];
				k = i;
			}
		Status[k] = 1; //k e o vertice de menor custo;
		VA = VA-1;
		//Verifica se precisa atualizar ou nao os vizinhos de k
		for(j = 0; j < Def::getNnodes(); j++)
			if((Status[j] == 0)&&(MAux::Topology[k][j] != 0)) {
				//O no j e nao marcado e vizinho do no k
				//Calcula O vetor de disponibilidade do enlace entre k e j
				for(int s = 0; s < Def::getSE(); s++)
					DispLink[s] = !MAux::Topology_S[s][k][j];
				custoLink = alpha*MAux::Caminho[k].at(j).get_comprimento()/MaiorComprimentoEnlace + (1.0-alpha)*Heuristics::calculateCostLink(DispLink, L);
				if(CustoVertice[k] + custoLink < CustoVertice[j]) {
					CustoVertice[j] = CustoVertice[k] + custoLink;
					Precedente[j] = k;
				}
			}
	}

	//Formar a rota:
	path = orN*Def::getNnodes()+deN;
	MAux::AllRoutes[path].clear();
	PathRev[0] = deN;
	hops = 0;
	j = deN;
	while(j != orN) {
		hops = hops+1;
		PathRev[hops] = Precedente[j];
		j = Precedente[j];
	}
	vector<Node*> r;
	r.clear();
	for(h = 0; h <= hops; h++)
		r.push_back(&MAux::Rede.at(PathRev[hops-h]));
	assert(r.at(0)->get_whoami() == orN && r.at(hops)->get_whoami() == deN);
	MAux::AllRoutes[path].push_back(new Route(r));

	delete []CustoVertice;
	delete []Precedente;
	delete []Status;
	delete []PathRev;
	delete []DispLink;
}

void RWA::FirstFit(const Route* route, const int NslotsReq, int& NslotsUsed, int& si) {
	si = -1;
	NslotsUsed = 0; //Valores nao permitidos
	int sum;
	for(int s = 0; s <= Def::getSE() - NslotsReq; s++) {
		sum = 0;
		for(int se = s; se < s + NslotsReq; se++) {
			if(CheckSlotAvailability(route, se))
				sum++;
			else
				break;
		}
		if(sum == NslotsReq) { //conseguiu alocar slots
			si = s;
			NslotsUsed = NslotsReq;
			break;
		}
	}
}

void RWA::FirstFitOpt(const Route* route, const int NslotsReq, int& NslotsUsed, int& si) {
	assert( (si == -1) && (NslotsUsed == 0) );
	int s, sum=0;
	for(int sOrd = 0; sOrd < Def::getSE(); sOrd++) {
		s = MAux::FFlists[NslotsReq]->at(sOrd);
		if(s <= Def::getSE()-NslotsReq) {
			//si e capaz de suportar a requisicao;
			sum = 0;
			for(int se = s; se < s + NslotsReq; se++)
				if(CheckSlotAvailability(route, se))
					sum++;
				else
					break;
		}
		if(sum == NslotsReq) { //O slot s pode atender a requisicao
			si = s;
			NslotsUsed = NslotsReq;
			break;
		}
	}
}

void RWA::MostUsed(const Route* route, const int NslotsReq, int& NslotsUsed, int& si) {
	int *vetSlotsUsed = new int[Def::getSE()];
	bool *vetDisp = new bool[Def::getSE()];

	//Checa a disponibilidade no caminho 'path' para cada slot s;
	for(int s = 0; s < Def::getSE(); s++)
		vetDisp[s] = RWA::CheckSlotAvailability(route, s);

	//Carrega vetSlotsUsed com o numero de enlaces ocupados em cada slot;
	int soma;
	for(int s = 0; s < Def::getSE(); s++) {
		if(vetDisp[s] == true)
			vetSlotsUsed[s] = sumOccupation(s);
		else
			vetSlotsUsed[s] = -1;
	}

	//Obtem o slot mais ocupado dentre os que podem receber a requisicao
	bool fit;
	double maxSoma = -1;//Smax*N*N+1;
	NslotsUsed = 0;
	si = -1;
	for(int s = 0; s <= Def::getSE()-NslotsReq; s++) {
		fit = true;
		soma = 0;
		for(int b = s; b < s + NslotsReq; b++)
			if(vetDisp[b] == true) {
				assert(vetSlotsUsed[b] >= 0);
				soma += vetSlotsUsed[b];
			} else {
				fit = false;
				break;
			}
		if(fit == true && soma > maxSoma) { //A requisicao se encaixa entre os slots s,s+1,...,s+NslotsReq-1 e nesta posicao havera a maior soma de enlaces ocupados na rede;
			maxSoma = soma;
			NslotsUsed = NslotsReq;
			si = s;
		}
	}
	delete []vetSlotsUsed;
	delete []vetDisp;
}

void RWA::Random(const Route* route, const int NslotsReq, int& NslotsUsed, int& si) {
	int soma=0, somaAlocacao=0, alocarSlot;
	bool *vetDisp = new bool[Def::getSE()];
	int *vetAloc = new int[Def::getSE()];
	for(int s = 0; s < Def::getSE(); s++)
		vetAloc[s] = 0;

	//Checa a disponibilidade no caminho 'path' para cada slot s;
	for(int s = 0; s < Def::getSE(); s++)
		vetDisp[s] = CheckSlotAvailability(route, s);
	//Carrega vetSlotsUsed com o numero de enlaces ocupados em cada slot;
	bool fit;
	NslotsUsed = 0;
	si = -1;
	for(int s = 0; s <= Def::getSE() - NslotsReq; s++) {
		fit = true;
		soma = 0;
		for(int b = s; b < s + NslotsReq; b++)
			if(vetDisp[b] == true)
				soma++;
			else {
				fit = false;
				break;
			}
		if(fit == true && soma == NslotsReq) {
			//A requisicao se encaixa entre os slots s,s+1,...,s+NslotsReq-1
			somaAlocacao++; // verifica quantas posicoes estao disponiveis para alocacao
			vetAloc[s] = 1; //posso alocar a partir desta posicao
		}
	}

	// fazer o sorteio do slot que sera alocada
	if(somaAlocacao > 0) {
		// se existir pelo menos um conjunto slot disponivel com o tamanho da solicitacao
		alocarSlot = rand()% somaAlocacao; // fazer alocacao no conjunto de slot dispovivel "alocarSlot"
		int s = 0;
		while ((s < Def::getSE()) && (alocarSlot >= 0)) {
			if(vetAloc[s] == 1)
				alocarSlot--;
			s++;
		}
		NslotsUsed = NslotsReq;
		si = s-1; // -1 porque foi incrementado no final do while
	} else {
		NslotsUsed = 0;
		si = -1;
	}
	delete []vetDisp;
	delete []vetAloc;
}

int RWA::sumOccupation(int s) {
	int soma=0;
	for(int origem = 0; origem < Def::getNnodes(); origem++)
		for(int destino = 0; destino < Def::getNnodes(); destino++)
			if( (MAux::Topology[origem][destino] > 0.0) && (MAux::Topology_S[s][origem][destino] == true) )
				//Se houver enlace entre origem e destino e o slot 's' estiver livre neste enlace
				soma++;
	return soma;
}

void RWA::LORModificado(const int orN, const int deN, const int L) {
	//L e a largura de banda (em numero de slots) da requisicao
	assert(orN != deN);
	int VA, i, j, k=0, path, h, hops;
	long double min;
	bool *DispLink = new bool[Def::getSE()];
	long double *CustoVertice = new long double[Def::getNnodes()];
	long double custoLink;
	int *Precedente = new int[Def::getNnodes()];
	int *PathRev = new int[Def::getNnodes()];
	bool *Status = new bool[Def::getNnodes()];
	long double MaiorComprimentoEnlace = 0;
	//Busca para todos os pares de no a rota mais curta:
	for(i = 0; i < Def::getNnodes(); i++) {
		if(i != orN)
			CustoVertice[i] = Def::MAX_DOUBLE;
		else
			CustoVertice[i] = 0.0;
		Precedente[i] = -1;
		Status[i] = 0;
	}
	//Busca Maior Enlace
	for (i = 0; i < Def::getNnodes(); i++) {
		for (int j = i+1; j < Def::getNnodes(); j++) {
			if (MAux::Topology[i][j] == 0) continue;
			if (MAux::Caminho[i].at(j).get_comprimento() > MaiorComprimentoEnlace)
				MaiorComprimentoEnlace = MAux::Caminho[i].at(j).get_comprimento();
		}
	}
	VA = Def::getNnodes();
	while(VA > 0) {
		//Procura o vertice de menor custo
		min = Def::MAX_DOUBLE;
		for(i = 0; i < Def::getNnodes(); i++)
			if((Status[i] == 0)&&(CustoVertice[i] < min)) {
				min = CustoVertice[i];
				k = i;
			}
		Status[k] = 1; //k e o vertice de menor custo;
		VA = VA-1;
		//Verifica se precisa atualizar ou nao os vizinhos de k
		for(j = 0; j < Def::getNnodes(); j++)
			if((Status[j] == 0)&&(MAux::Topology[k][j] != 0)) {
				//O no j e nao marcado e vizinho do no k
				//Calcula O vetor de disponibilidade do enlace entre k e j
				for(int s = 0; s < Def::getSE(); s++)
					DispLink[s] = !MAux::Topology_S[s][k][j];
				custoLink = MAux::Caminho[k].at(j).get_comprimento()/MaiorComprimentoEnlace - Heuristics::calculateCostLink(DispLink, L) + 1;
				if(CustoVertice[k] + custoLink < CustoVertice[j]) {
					CustoVertice[j] = CustoVertice[k] + custoLink;
					Precedente[j] = k;
				}
			}
	}

	//Formar a rota:
	path = orN*Def::getNnodes()+deN;
	MAux::AllRoutes[path].clear();
	PathRev[0] = deN;
	hops = 0;
	j = deN;
	while(j != orN) {
		hops = hops+1;
		PathRev[hops] = Precedente[j];
		j = Precedente[j];
	}
	vector<Node*> r;
	r.clear();
	for(h = 0; h <= hops; h++)
		r.push_back(&MAux::Rede.at(PathRev[hops-h]));
	assert(r.at(0)->get_whoami() == orN && r.at(hops)->get_whoami() == deN);
	MAux::AllRoutes[path].push_back(new Route(r));

	delete []CustoVertice;
	delete []Precedente;
	delete []Status;
	delete []PathRev;
	delete []DispLink;
}
