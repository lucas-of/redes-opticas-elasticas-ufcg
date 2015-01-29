#include "Def.h"
#include "math.h"
#include "Constantes.h"
#include <limits>

//Inicializa constantes estáticas
vector<double> Def::LaNet(0);
vector<int> Def::GrauNo(0);
double Def::MAX_DOUBLE = std::numeric_limits<double>::max();
int Def::MAX_INT = std::numeric_limits<int>::max();
double Def::MAX_LONGDOUBLE = std::numeric_limits<long double>::max();
int Def::Nnodes = 0;
long double Def::netOccupancy = 0.0;
const int Def::numEsquemasDeModulacao = 3;
long double Def::numHopsPerRoute = 0.0;
long double Def::numReq = 0.0;
long double Def::numReq_Bloq = 0.0;
long double Def::numReqMax = 0.0;
long double Def::numSlots_Bloq = 0.0;
long double Def::numSlots_BloqPorOSNR = 0.0;
long double Def::numReq_BloqPorOSNR = 0.0;
long double Def::numSlots_Req = 0.0;
long double Def::PossiveisTaxas[] = { 10E9, 40E9, 100E9, 160E9 }; //em Gbps
const int Def::numPossiveisTaxas = 4;
long double Def::numReq_Taxa[Def::numPossiveisTaxas] = {0};
long double Def::numReqBloq_Taxa[Def::numPossiveisTaxas] = {0};
long double Def::tempoTotal_Taxa[Def::numPossiveisTaxas] = {0};
int Def::SE = 0;
int Def::SR = 0;
long double Def::Pin=1.0;
long double Def::Pref=1.0;
long double Def::OSNRin=30.0;
long double Def::Lsss=5.0;
long double Def::DistA=75.0;
Def::Arquitetura Def::arquitetura=Def::BS;
long double Def::lambda = 1550E-9;
long double Def::Bslot = 12.5;
long double Def::Bref = 12.5;
long double Def::Famp = General::dB((long double) 5.0);

void Def::setBslot(double Bslot) {
    assert (Bslot > 0);
    Def::Bslot = Bslot * 1E9;
}

void Def::setBref(double Bref) {
    assert (Bref > 0);
    Def::Bref = Bref * 1E9;
}

long double Def::getlambda() {
    return lambda;
}

void Def::setOSNR(long double osnr) {
    assert(osnr >= 0);
    Def::OSNRin = osnr;
}

void Def::clearGrauNo() {
    GrauNo.clear();
}

int Def::getGrauNo(int No) {
    assert(No < Nnodes);
    return GrauNo.at(No);
}

double Def::getLaNet(int Lr) {
    assert(Lr>0 && Lr <= SR);
    return LaNet.at(Lr);
}

double Def::getlimiarOSNR(EsquemaDeModulacao Esquema, long double TaxaDeBit) {
    //cout << (0.5*TaxaDeBit*get_snrb(Esquema)/Def::get_Bref()) << endl;
    return (0.5*TaxaDeBit*get_snrb(Esquema)/Def::get_Bref());
}

long double Def::get_Bref() {
    return Bref;
}

int Def::getSE() {
    return SE;
}

long double Def::get_snrb(EsquemaDeModulacao Esq) {
    switch (Esq) {
        case _4QAM:
            return 6.8;
        case _16QAM:
            return 10.5;
        case _64QAM:
            return 14.8;
        default:
            return 0;
    }
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

void Def::setGrauNo(int Grau) {
    assert(GrauNo.size() < (unsigned) Nnodes);
    GrauNo.push_back(Grau);
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

void Def::set_Pin(long double p) {
    Pin = 1E-3 * General::dB(p); //conversao para Watt
}

void Def::set_Lsss(long double l){
    assert (l >0);
    Lsss=l;
}

void Def::set_DistaA(long double d){
    DistA = d;
}

void Def::set_Arquitetura(Arquitetura a){
    arquitetura=a;
}

long double Def::get_Pin(){
    return Pin;
}

long double Def::get_OSRNin(){
    return OSNRin;
}

long double Def::get_Lsss(){
    return Lsss;
}

long double Def::get_DistaA() {
    return DistA;
}

long double Def::get_Bslot() {
    return Bslot;
}

long double Def::get_Famp() {
    return Famp;
}

int Def::get_numPossiveisTaxas() {
    return numPossiveisTaxas;
}

Def::Arquitetura Def::get_Arquitetura() {
    return arquitetura;
}

long double Def::get_Pref() {
    return Pref;
}

void Def::set_Pref(long double P) {
    Pref = 1E-3*General::dB(P);
}
