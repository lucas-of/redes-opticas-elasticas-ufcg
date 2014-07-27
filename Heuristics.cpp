#include "Heuristics.h"
#include "limits.h"

///////////////////////////////////////////////////////////////////////////////////
void Heuristics::FFO_convencional(vector<int>** FFlists)  //Assumindo FF convencinal com a estrutura do FFO
{
    cout<<"Heuristics::FFO_convencional"<<endl;
    for(int Lr = 1; Lr <= Def::getSR(); Lr++)
    {
        FFlists[Lr]->clear();
        for(int si = 0; si < Def::getSE(); si++)
            FFlists[Lr]->push_back(si);
    }
}

// ------------------------------------------------------------------------------ //
void Heuristics::FFO_extremos(vector<int>** FFlists) //Assumindo FF extremos com a estrutura do FFO
{
    cout<<"Heuristics::FFO_extremos"<<endl;
    for(int Lr = 1; Lr <= Def::getSR(); Lr++)
    {
        FFlists[Lr]->clear();
        int minSi = 0, maxSi = Def::getSE()-Lr;
        while(maxSi >= minSi)
        {
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
}

// ------------------------------------------------------------------------------ //
void Heuristics::FFO_invertido(vector<int>** FFlists)
{
    cout<<"Heuristics::FFO_invertido"<<endl;
    for(int Lr = 1; Lr <= Def::getSR(); Lr++)
    {
        FFlists[Lr]->clear();
        for(int si = 0; si < Def::getSE(); si++)
            FFlists[Lr]->push_back(Def::getSE()-1-si);
    }
}

// ------------------------------------------------------------------------------ //
void Heuristics::FFO_metrica(vector<int>** FFlists) //Cria uma lista através de alguma métrica
{
    cout<<"Heuristics::FFO_metrica"<<endl;
    //Cria a lista FFopt
    int pos, posMet;
    int *CapFut = new int[(Def::getSR()+1)*(Def::getSR()+1)*Def::getSE()];
    bool *Disp = new bool[Def::getSE()];
    int *Met = new int[(Def::getSR()+1)*Def::getSE()];
    General::setVector<int>(Met, (Def::getSR()+1)*Def::getSE(), 0);
    int Lr, Lf, si, se;
    for(Lr = 1; Lr <= Def::getSR(); Lr++) //Lr = Largura da requisição
        for(Lf = 1; Lf <= Def::getSR(); Lf++) //Lf = Largura de uma requisição futura
            for(si = 0; si < Def::getSE(); si++)  //s = slot a ser utilizado
            {
                pos = si*(Def::getSR()+1)*(Def::getSR()+1) + Lr*(Def::getSR()+1) + Lf;
                General::setVector<bool>(Disp, Def::getSE(), true);
                if(si+Lr <= Def::getSE())  // A requisição pode ser inserida nos slots si, si+1,...si+Lr-1
                {
                    for(se = si; se < si+Lr; se++)
                        Disp[se] = false;
                    CapFut[pos] = calcNumFormAloc(Lf, Disp); //Quantas formas de alocação
                    //CapFut[pos] = calcNumAloc(Lf, Disp); //Quantas alocações realmente
                }
                else
                    CapFut[pos] = -1;
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
    int best_si, max, posMetMax;
    for(Lr = 1; Lr <= Def::getSR(); Lr++)  //Lr = Largura da requisição
    {
        FFlists[Lr]->clear();
        do
        {
            best_si = -1;
            max = - INT_MAX;
            for(si = 0; si < Def::getSE(); si++)  //s = slot a ser utilizado
            {
                posMet = Lr*Def::getSE() + si;
                if(Met[posMet] > max)
                {
                    max = Met[posMet];
                    posMetMax = posMet;
                    best_si = si;
                }
            }
            FFlists[Lr]->push_back(best_si);
            Met[posMetMax] = - INT_MAX;
        }
        while(best_si != -1);
    }
    //Limpeza:
    delete []CapFut;
    delete []Disp;
    delete []Met;
}

///////////////////////////////////////////////////////////////////////////////////
void Heuristics::TestFFlists(vector<int>** FFlists)
{
    //Testes de segurança:
    //Checar se as listas estão completas, i.e., se elas possuem todos os slots diferentes
    for(int Lr = 1; Lr <= Def::getSR(); Lr++)  //Lr = Largura da requisição
    {
        assert(FFlists[Lr]->size() == Def::getSE());
        for(int si = 0; si < Def::getSE(); si++) //s = slot a ser utilizado
            for(int se = si+1; se < Def::getSE(); se++)
                assert(FFlists[Lr]->at(si) != FFlists[Lr]->at(se));
    }
}

// ------------------------------------------------------------------------------ //
void Heuristics::PrintFFlists(vector<int>** FFlists) //Imprimir a Lista otimizada:
{
    cout<<endl<<"Lista Otimizada:"<<endl;
    for(int Lr = 1; Lr <= Def::getSR(); Lr++)  //Lr = Largura da requisição
    {
        cout<<endl<<"Lista("<<Lr<<": ";
        General::printAnsiVector<int>(*FFlists[Lr]);
        cout<<endl;
    }
}

///////////////////////////////////////////////////////////////////////////////////
long double Heuristics::calculateCostLink(bool* Disp, int L)  //L é a largura da requisição
{
    //return 1.0; //Dijkstra Normal
    /*
    long double alpha = 0.45;
    return exp( -alpha*Heuristics::calcNumFormAloc(L, Disp)/(Def::getSE()-L+1.0) );
    */


    //Dijkstra que usa o número de formas para a requisição apenas:
    int numFormsL = Heuristics::calcNumFormAloc(L, Disp);
    return (long double) 1.0/(numFormsL+1.0);

}

// ------------------------------------------------------------------------------ //
long double Heuristics::calculateCostRouting(int hops, bool* Disp, int L)  //L é a largura da requisição
{
    /*
    //Dijkstra normal, que usa o número de hops apenas
    return hops;
    */
    /*
    //Dijkstra que usa o número de formas para a requisição apenas:
    int numFormsL = Heuristics::calcNumFormAloc(L, Disp);
    return (long double) 1.0/(numFormsL+1.0);
    */

    //Dijkstra que usa a ponderação entre o número de hops e o o número de formas:
    long double alpha = 0.9999;
    int numFormsL = Heuristics::calcNumFormAloc(L, Disp);
    if(numFormsL == 0)
        return Def::MAX_LONGDOUBLE/2.0;
    return (long double) alpha*hops + (1.0-alpha)*((long double)1.0/(numFormsL+1.0));

}

///////////////////////////////////////////////////////////////////////////////////
int Heuristics::calcNumFormAloc(int L, bool* Disp)  //L indica a largura da requisição
{
    int sum = 0, si, se; //si é o slot inicial da alocação, que vai de 0 até SE-L
    for(si = 0; si <= Def::getSE()-L; si++)
    {
        for(se = si; se < si+L; se++) //se checa se todos os slots de si até si+L-1 estão disponíveis
            if(Disp[se] == false)
                break;
        if(se == si+L) // Os slots si,si+1,...,si+Lf-1 estão disponíveis
            sum++;
    }
    return sum;
}
