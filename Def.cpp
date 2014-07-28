#include "Def.h"
#include <limits>

//Inicializa constantes estáticas
vector<double> Def::LaNet(0);
double Def::MAX_DOUBLE = std::numeric_limits<double>::max();
int Def::MAX_INT = std::numeric_limits<int>::max();
double Def::MAX_LONGDOUBLE = std::numeric_limits<long double>::max();
int Def::Nnodes = 0;
long double Def::netOccupancy = 0.0;
long double Def::numHopsPerRoute = 0.0;
long double Def::numReq = 0.0;
long double Def::numReq_Bloq = 0.0;
long double Def::numReqMax = 0.0;
long double Def::numSlots_Bloq = 0.0;
long double Def::numSlots_Req = 0.0;
int Def::SE = 0;
int Def::SR = 0;


double Def::getLaNet(int Lr) {
    assert(Lr>0 && Lr <= SR);
    return LaNet.at(Lr);
}

int Def::getSE() {
    return SE;
}

int Def::getSR() {
    return SR;
}

int Def::getNnodes() {
    return Nnodes;
}

long double Def::getNumReqMax() {
    return numReqMax;
}

void Def::setLaCheck(double la) {
    //Certificar que está normalizado

    double sum = 0.0;
    for (int Lr = 1; Lr <= SR; Lr++)
        sum += LaNet.at(Lr);
    assert(sum > 0.999*la && sum < 1.001*la);
}

void Def::setLaRandom(double la) {
    //Tráfego Aleatório para requisições s = 1, 2, ..., SR
    //A soma de todos os LaNet[i] deve dar la;

    cout << "La_slot Aleatorio" << endl;
    assert(SR > 0);
    LaNet.clear();
    LaNet.push_back(-1.0); //Lr=0 não pode ser requisitado
    double sum = 0.0;
    for(int Lr = 1; Lr <= SR; Lr++) {
        LaNet.push_back(General::uniforme(0.0,1.0));
        sum += LaNet.at(Lr);
    }
    for(int Lr = 1; Lr <= SR; Lr++)
        LaNet.at(Lr) = (LaNet.at(Lr)/sum)*la;
    setLaCheck(la); //Checa
}

void Def::setLaUniform(double la) {
    //Tráfego Uniforme entre requisições s = 1, 2, ..., SR
    //A soma de todos os LaNet[i] deve dar la;

    cout << "La_slot uniforme" << endl;
    assert(SR > 0);
    LaNet.clear();
    LaNet.push_back(-1.0); //Lr=0 não pode ser requisitado
    for(int Lr = 1; Lr <= SR; Lr++)
        LaNet.push_back((double)la/SR);
    setLaCheck(la); //Checa
}

void Def::setNnodes(int x) {
    Nnodes = x;
}

void Def::setNumReqMax(long double x) {
    assert(x > 0);
    numReqMax = x;
}

void Def::setSE(int x) {
    assert(x > 0);
    SE = x;
}

void Def::setSR(int x) {
    SR = x;
}
