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
long double Def::Pin=0.0;
long double Def::OSNRin=0.0;
long double Def::Lsss=0.0;
long double Def::Cenl=0.0;//Comprimento dos enlaces, uma matriz
long double Def::DistA=0.0;
int Def::arquitetura=0;

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

void Def::set_Pin(long double p){
    Pin=p;
}

void Def::set_OSNRin(long double o){
    OSNRin=o;
}

void Def::set_Lsss(long double l){
    Lsss=l;
}

void Def::set_Cenl(long double c){//É dado por uma matriz
    Cenl=c;
}

void Def::set_DistaA(long double d){
    DistaA=d;
}

void Def::set_Arquiterura(int a){
    arquiterura=a;
}


long double Def::get_Pin(){
    return Pin;
}

long double Def::get_OSRNin(){
    return OSRNin;
}

long double Def::get_Lsss(){
    return Lsss;
}

long double Def::get_Cenl(){//É dado por uma matriz
    return Cenl;
}

long double Def::get_DistaA(){
    return DistaA;
}

int Def::set_Arquitetura(){
    return arquitetura;
}
