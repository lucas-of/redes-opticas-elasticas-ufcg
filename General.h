#ifndef General_H
#define General_H

#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <vector>
using namespace std;
#include <math.h>

////////////////////////////////////////////////////////////////
class General
{
public:
    template <class T>
    static void setVector(T* V, int size, T value);
    template <class T>
    static void printAnsiVector(vector<T>&);
    static long double exponential(long double L);
    static long double uniforme(long double xmin, long double xmax);
    template <class T>
    static const T& ABS(const T& x);
    template <class T>
    static const T& MAX (const T& a, const T& b);
    template <class T>
    static const T& MIN (const T& a, const T& b);
    //template <class T>
    //static ofstream&(const vector<T>&);
private:

};

////////////////////////////////////////////////////////////////
template <class T>
void General::setVector(T* V, int size, T value)
{
    for(int i = 0; i < size; i++)
        V[i] = value;
}

// -------------------------------------------------------------------------- //
template <class T>
void General::printAnsiVector(vector<T>& v)
{
    for(unsigned i = 0; i < v.size(); i++)
        cout<<v.at(i)<<" ";
}

////////////////////////////////////////////////////////////////////////////////
template <class T>
const T& General::ABS(const T& x)
{
    return (x>0)?x:-x;
}

// -------------------------------------------------------------------------- //
template <class T>
const T& General::MAX (const T& a, const T& b)
{
    return (a>b)?a:b;
}

// -------------------------------------------------------------------------- //
template <class T>
const T& General::MIN (const T& a, const T& b)
{
    return (a<b)?a:b;
}

#endif
