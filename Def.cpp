#include "Def.h"
#include <limits>

int Def::Nnodes = 0;
int Def::SR = 0;
int Def::SE = 0;
vector<double> Def::LaNet(0);
long double Def::numReqMax = 0.0;
long double Def::numReq = 0.0;
long double Def::numReq_Bloq = 0.0;
long double Def::numSlots_Bloq = 0.0;
long double Def::numSlots_Req = 0.0;
long double Def::numHopsPerRoute = 0.0;
long double Def::netOccupancy = 0.0;

int Def::MAX_INT = std::numeric_limits<int>::max();
double Def::MAX_DOUBLE = std::numeric_limits<double>::max();
double Def::MAX_LONGDOUBLE = std::numeric_limits<long double>::max();

////////////////////////////////////////////////////////////////
void Def::setNnodes(int x)
{
    Nnodes = x;
}

// ------------------------------------------------- //
int Def::getNnodes()
{
    return Nnodes;
}

////////////////////////////////////////////////////////////////
void Def::setSR(int x)
{
    SR = x;
}

// ------------------------------------------------- //
int Def::getSR()
{
    return SR;
}

// ------------------------------------------------- //
void Def::setSE(int x)
{
    assert(x > 0);
    SE = x;
}

// ------------------------------------------------- //
int Def::getSE()
{
    return SE;
}

// ------------------------------------------------- //
void Def::setNumReqMax(long double x)
{
    assert(x > 0);
    numReqMax = x;
}

// ------------------------------------------------- //
long double Def::getNumReqMax()
{
    return numReqMax;
}

///////////////////////////////////////////////////////////////////////////////////
void Def::setLaUniform(double la) 	//Tráfego Uniforme entre requisições s=1,2,...SR: //A soma de todos os LaNet[i] deve dar la;
{
    cout<<"La_slot uniforme"<<endl;
    assert(SR > 0);
    LaNet.clear();
    LaNet.push_back(-1.0); //Lr=0 não pode ser requisitado
    for(int Lr = 1; Lr <= SR; Lr++)
        LaNet.push_back((double)la/SR);
    //Fazer uma checagem:
    setLaCheck(la);
}

// ---------------------------------------------------------- //
void Def::setLaRandom(double la)  //Tráfego Aleatório para requisições s=1,2,...SR: //A soma de todos os LaNet[i] deve dar la;
{
    cout<<"La_slot Aleatorio"<<endl;
    assert(SR > 0);
    LaNet.clear();
    LaNet.push_back(-1.0); //Lr=0 não pode ser requisitado
    double sum = 0.0;
    for(int Lr = 1; Lr <= SR; Lr++)
    {
        LaNet.push_back(General::uniforme(0.0,1.0));
        sum += LaNet.at(Lr);
    }
    for(int Lr = 1; Lr <= SR; Lr++)
        LaNet.at(Lr) = (LaNet.at(Lr)/sum)*la;
    //Fazer uma checagem:
    setLaCheck(la);
}

// ---------------------------------------------------------- //
void Def::setLaManual(double la)
{
    cout<<"La_slot Manual"<<endl;
    assert(SR > 0);
    LaNet.clear();
    LaNet.push_back(-1);
    LaNet.push_back(3.60951);
    LaNet.push_back(3.63336);
    LaNet.push_back(0.912896);
    LaNet.push_back(0.789896);
    LaNet.push_back(2.24586);
    LaNet.push_back(0.821782);
    LaNet.push_back(2.75184);
    LaNet.push_back(0.234856);
}

// ------------------------------------------------- //
void Def::setLaCheck(double la) //Certificar que está correto
{
    double sum = 0.0;
    for(int Lr = 1; Lr <= SR; Lr++)
        sum += LaNet.at(Lr);
    assert(sum > 0.999*la && sum < 1.001*la);
}

// ------------------------------------------------- //
double Def::getLaNet(int Lr)
{
    assert(Lr>0 && Lr <= SR);
    return LaNet.at(Lr);
}
