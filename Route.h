#ifndef Route_H
#define Route_H

#include <assert.h>
#include <vector>
#include <iostream>
using namespace std;


//////////////////////////////////////////////
class Route
{
public:
    Route(vector<int>&);
    int getOrN() const;
    int getDeN() const;
    unsigned getNhops() const;
    int getNode(unsigned) const;
    void print() const;
private:
    vector<int> Path;
};

#endif
