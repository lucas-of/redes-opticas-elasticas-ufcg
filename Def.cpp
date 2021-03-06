#include "Def.h"
#include "math.h"
#include "Constantes.h"
#include "Main_Auxiliar.h"
#include <limits>

//Inicializa constantes estáticas
vector<int> Def::GrauNo(0);
double Def::MAX_DOUBLE = std::numeric_limits<double>::max();
int Def::MAX_INT = std::numeric_limits<int>::max();
double Def::MAX_LONGDOUBLE = std::numeric_limits<long double>::max();
int Def::Nnodes;
const int Def::numEsquemasDeModulacao = 3;
long double Def::numReqBloqMin = 0.0;
long double Def::numReqMax = 1E7; //dez milhoes de requisicoes
const int Def::numPossiveisTaxas = 5;
long double Def::PossiveisTaxas[Def::numPossiveisTaxas] = {10, 40, 100, 160, 400}; //em Gbps
int Def::SE = 0;
int Def::SR = 0;
int Def::maxSim_Bigode = 0;
long double Def::Pin=1.0;
long double Def::Pref=1.0;
long double Def::Lsss=5.0;
long double Def::freq = 193.4E12;
Def::Arquitetura Def::arquitetura=Def::BS;
long double Def::lambda = 1550E-9;
long double Def::Bslot = 12.5;
long double Def::Bref = 12.5;
long double Def::Famp = General::dB((long double) 5.0);

Def::Def(Particula *Part) {
	P = Part;
	DistA=80;
	OSNRin=30;
	numReqAceit_Esquema = new long double[numEsquemasDeModulacao];
	numReq_Taxa = new long double[numPossiveisTaxas];
	numReqBloq_Taxa = new long double[numPossiveisTaxas];
	Topology_S = new bool[Nnodes*Nnodes*SE];
	for (int i=0 ; i < Def::getSE(); i++)
		for (int j=0; j < Def::getNnodes() ; j++)
			for (int k = 0; k < Def::getNnodes(); k++)
				Topology_S[i*Def::Nnodes*Def::Nnodes + Def::Nnodes*j + k] = false;

	numSlots_Bloq = 0.0;
	numSlots_BloqPorOSNR = 0.0;
	numReq_BloqPorOSNR = 0.0;
	numSlots_Req = 0.0;
	numReq_Bloq = 0.0;
	numHopsPerRoute = 0.0;
	numReq = 0.0;
	netOccupancy = 0.0;
	tempoTotal_Taxa = new long double[numPossiveisTaxas];
	taxaTotal_Esquema = new long double[numEsquemasDeModulacao];
	taxaTotal = 0;
}

Def::~Def() {
	delete[] numReqAceit_Esquema;
	delete[] numReq_Taxa;
	delete[] numReqBloq_Taxa;
	delete[] Topology_S;
	delete[] tempoTotal_Taxa;
	delete[] taxaTotal_Esquema;
}

void Def::setPossiveisTaxas() {
	for (int i = 0; i < Def::numPossiveisTaxas; i++)
		Def::PossiveisTaxas[i] = Def::PossiveisTaxas[i]*1E9;
}

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
	return General::lin(0.5*TaxaDeBit*get_snrb(Esquema)/Def::get_Bref());
}

long double Def::get_Bref() {
	return Bref;
}

int Def::getSE() {
	return SE;
}

long double Def::getNumReqMax() {
	return numReqMax;
}

long double Def::get_snrb(EsquemaDeModulacao Esq) {
	switch (Esq) {
		case _4QAM:
			return General::dB(6.8);
		case _16QAM:
			return General::dB(10.5);
		case _64QAM:
			return General::dB(14.8);
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

long double Def::getNumReqBloqMin() {
	return numReqBloqMin;
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

	//cout << "La_slot uniforme" << endl;
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

void Def::setNumReqBloqMin(long double x) {
	assert(x > 0);
	numReqBloqMin = x;
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

long double Def::get_freq() {
	return freq;
}
