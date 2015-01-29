#ifndef General_H
#define General_H

#include <assert.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <vector>

using namespace std;

class General { //classe Geral, serve a diversos usos. usa template e metodos static
    public:
        template <class T> static const T& ABS(const T& x); //retorna o módulo de T
        static long double exponential(long double L); //gera valor regido por dist. exponencial
        template <class T> static const T& MAX (const T& a, const T& b); //retorna o máximo entre A e B
        template <class T> static const T& MIN (const T& a, const T& b); //retorna o mínimo entre A e B
        template <class T> static void printAnsiVector(vector<T>&); //imprime um vetor de T
        template <class T> static void setVector(T* V, int size, T value); //inicializa vetor de comrpimento size com value
        template <class T> static long double dB(T x) { return pow(10,0.1*x); }
        template <class T> static long double lin(T x, T y) { return 10*(log10(x) - log10(y)); };
        static long double uniforme(long double xmin, long double xmax); //gera valor regido por dist. unif. entre xmin e xmax
};

//TEMPLATES DEVEM SER IMPLEMENTADOS NO HEADER

template <class T> const T& General::ABS(const T& x) {
    return (x>0)?x:-x;
}

template <class T> const T& General::MAX (const T& a, const T& b) {
    return (a>b)?a:b;
}

template <class T> const T& General::MIN (const T& a, const T& b) {
    return (a<b)?a:b;
}

template <class T> void General::printAnsiVector(vector<T> &v) {
    for(int i = 0; i < v.size(); i++)
        std::cout << v.at(i) << " ";
}

template <class T> void General::setVector(T* V, int size, T value) {
    for(int i = 0; i < size; i++)
        V[i] = value;
}

#endif
