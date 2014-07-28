#include "Heuristics.h"
#include "limits.h"

long double Heuristics::calculateCostLink(bool* Disp, int L) {
    //L e a largura da requisicao
    //Disp e uma matriz indicando a disponibilidade dos slots do enlace

    //return 1.0; //Dijkstra normal usa custo padrao

    //long double alpha = 0.45; return exp(-alpha*Heuristics::calcNumFormAloc(L,Disp)/(Def::getSE()-L+1.0)); //Dijkstra que usa ponderacao entre o numero de formas de alocar

    //Dijkstra que usa o numero de formas para a requisicao apenas:
    int numFormsL = Heuristics::calcNumFormAloc(L, Disp);
    return (long double) 1.0/(numFormsL+1.0);
}

long double Heuristics::calculateCostRouting(int hops, bool* Disp, int L)  {
    //hops e o numero de nos na rota
    //L e a largura da requisicao
    //Disp e uma matriz indicando a disponibilidade dos slots do enlace

    //return hops; //Dijkstra normal, que usa o numero de hops apenas

    //return (long double) 1.0/(Heuristics::calcNumFormAloc(L, Disp)+1.0); //Dijkstra que usa o numero de formas para a requisicao

    //Dijkstra que usa a ponderação entre o número de hops e o o número de formas:
    long double alpha = 0.9999; //de onde saiu?
    int numFormsL = Heuristics::calcNumFormAloc(L, Disp);
    if(numFormsL == 0)
        return Def::MAX_LONGDOUBLE/2.0; //infinito
    return (long double) alpha*hops + (1.0-alpha)*((long double)1.0/(numFormsL+1.0));
}

int Heuristics::calcNumFormAloc(int L, bool* Disp) {
    //L e a largura da requisicao
    //Disp e uma matriz indicando a disponibilidade dos slots do enlace

    int NumFormas = 0, SlotInicial, Slot;
    for(SlotInicial = 0; SlotInicial <= Def::getSE()-L; SlotInicial++) {
        for(Slot = SlotInicial; Slot < SlotInicial+L; Slot++) //Checa se os slots [SlotInicial : SlotInicial+L] estao disponiveis
            if(Disp[Slot] == false) //Slot ocupado
                break;
        if(Slot == SlotInicial+L) // Os slots estao disponiveis
            NumFormas++;
    }
    return NumFormas;
}

void Heuristics::FFO_convencional(vector<int>** FFlists)  {
    //Assumindo FF convencinal como a estrutura do FFO

    cout << "Heuristics::FFO_convencional" << endl;

    for(int Lr = 1; Lr <= Def::getSR(); Lr++) { //SR e o numero maximo de slots requisitados
        FFlists[Lr]->clear();
        for(int si = 0; si < Def::getSE(); si++) //SE e o numero de slots por enlace
            FFlists[Lr]->push_back(si);
    }
    /*cria uma "matriz" da forma
     *      0   1   2   3
     *      0   1   2   3
     *      0   1   2   3
     *      0   1   2   3   [...]   */
}

void Heuristics::FFO_extremos(vector<int>** FFlists) {
    //Assumindo FF extremos como a estrutura do FFO

    cout<<"Heuristics::FFO_extremos"<<endl;

    for(int Lr = 1; Lr <= Def::getSR(); Lr++) { //SR e o numero maximo de slots requisitados
        FFlists[Lr]->clear();
        int minSi = 0, maxSi = Def::getSE()-Lr; //SE e o numero de slots por enlace

        while(maxSi >= minSi) {
            FFlists[Lr]->push_back(minSi);
            if(maxSi != minSi)
                FFlists[Lr]->push_back(maxSi);
            minSi += 1;
            maxSi -= 1;
        }

        //Completar a lista com os slots que faltam:
        for(int si = Def::getSE()-Lr+1; si < Def::getSE(); si++)
            FFlists[Lr]->push_back(si);
    }

    /* cria uma "matriz" da forma
     *      0   SE-1    1   SE-2    [...]
     *      0   SE-2    1   SE-3    [...]   SE-1
     *      0   SE-3    1   SE-4    [...]   SE-2    SE-1    */
}

void Heuristics::FFO_invertido(vector<int>** FFlists) {
    //Assumindo FF invertido como a estrutura do FFO

    cout << "Heuristics::FFO_invertido" << endl;
    for(int Lr = 1; Lr <= Def::getSR(); Lr++) {
        FFlists[Lr]->clear();
        for(int si = 0; si < Def::getSE(); si++)
            FFlists[Lr]->push_back(Def::getSE()-1-si);
    }

    /* cria uma "matriz" da forma
     *      SE-1    SE-2    [...]   1   0
     *      SE-1    SE-2    [...]   1   0
     *      SE-1    SE-2    [...]   1   0   */
}

void Heuristics::FFO_metrica(vector<int>** FFlists) {
    //Assumindo FF de acordo com alguma metrica como a estrutura do FFO

    //Cria a lista FFopt
    cout<<"Heuristics::FFO_metrica"<<endl;
    int pos, posMet; //pos = posicao na matriz, posMet = posicao na matriz de metricas
    int *CapFut = new int[(Def::getSR()+1)*(Def::getSR()+1)*Def::getSE()]; //formas de alocar requisição, organizados... (de alguma maneira O.o)
    bool *Disp = new bool[Def::getSE()];
    int *Met = new int[(Def::getSR()+1)*Def::getSE()];
    General::setVector<int>(Met, (Def::getSR()+1)*Def::getSE(), 0);
    int Lr, Lf, si, se; /*Lr = largura da requisição
                          Lf = largura de uma requisição futura
                          si, se = slot */

    for(Lr = 1; Lr <= Def::getSR(); Lr++)
        for(Lf = 1; Lf <= Def::getSR(); Lf++)
            for(si = 0; si < Def::getSE(); si++) {
                pos = si*(Def::getSR()+1)*(Def::getSR()+1) + Lr*(Def::getSR()+1) + Lf;
                General::setVector<bool>(Disp, Def::getSE(), true);
                if (si+Lr <= Def::getSE()) { // A requisição pode ser inserida nos slots si, si+1,...si+Lr-1
                    for(se = si; se < si+Lr; se++) Disp[se] = false;
                    CapFut[pos] = calcNumFormAloc(Lf, Disp); //Quantas formas de alocação
                } else CapFut[pos] = -1; //impossivel alocar requisição
                posMet = Lr*Def::getSE() + si;
                Met[posMet] += CapFut[pos]; //1a métrica: Apenas o somatório
                //Met[posMet] += Lf*CapFut[pos]; //2a métrica: O produto pela banda
            }

    /*
        //Imprimir a Capacidade Futura:
        Lr = 4;
        for(si = 0; si < Def::getSE(); si++){
            cout<<endl;
            for(Lf = 1; Lf <= Def::getSR(); Lf++){
                pos = si*(Def::getSR()+1)*(Def::getSR()+1) + Lr*(Def::getSR()+1) + Lf;
                cout<<CapFut[pos]<<" ";
            }
        }

        //Imprimir as Métricas:
        cout<<endl<<"Metrica para Otimizar:"<<endl;
        for(Lr = 1; Lr <= Def::getSR(); Lr++){
            cout<<endl;
            for(si = 0; si < Def::getSE(); si++){
                posMet = Lr*Def::getSE() + si;
                cout<<Met[posMet]<<" ";
            }
        }
        cout<<endl;
    */

    //Obtenção da lista otimizada:
    int best_si, max, posMetMax=0;
    for(Lr = 1; Lr <= Def::getSR(); Lr++) {
        //Lr = Largura da requisição
        FFlists[Lr]->clear(); //limpa a matriz
        do {
            best_si = -1;
            max = -INT_MAX;
            for(si = 0; si < Def::getSE(); si++) { //encontra o maior valor de acordo com a métrica
                //si = slot a ser utilizado
                posMet = Lr*Def::getSE() + si;
                if(Met[posMet] > max) {
                    max = Met[posMet];
                    posMetMax = posMet;
                    best_si = si;
                }
            }
            FFlists[Lr]->push_back(best_si); //joga maior valor na matriz otimizada
            Met[posMetMax] = - INT_MAX;
        } while(best_si != -1);
    }

    delete []CapFut;
    delete []Disp;
    delete []Met;
}

void Heuristics::TestFFlists(vector<int>** FFlists) {
    //Checa se as listas estão completas, i.e., se elas possuem todos os slots diferentes
    for(int Lr = 1; Lr <= Def::getSR(); Lr++) {
        //Lr = Largura da requisição
        assert(FFlists[Lr]->size() == (unsigned)Def::getSE());
        for(int si = 0; si < Def::getSE(); si++)
            //si = slot em teste
            for(int se = si+1; se < Def::getSE(); se++)
                assert(FFlists[Lr]->at(si) != FFlists[Lr]->at(se));
    }
}

void Heuristics::PrintFFlists(vector<int>** FFlists) {
    //Imprime a lista otimizada

    cout << endl << "Lista Otimizada:" << endl;
    for(int Lr = 1; Lr <= Def::getSR(); Lr++) {
        //Lr = Largura da requisição
        cout << endl << "Lista (" << Lr << ": ";
        General::printAnsiVector<int>(*FFlists[Lr]);
        cout << endl;
    }
}
