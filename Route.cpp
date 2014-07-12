#include "Route.h"

////////////////////////////////////////////////////////////////
Route::Route(vector<int>& path)
    :Path(path)
{
}

////////////////////////////////////////////////////////////////
int Route::getOrN() const
{
    assert(Path.size()>0);
    return Path.front();
}

// ------------------------------------------------ //
int Route::getDeN() const
{
    return Path.back();
}

// ------------------------------------------------ //
unsigned Route::getNhops() const
{
    return Path.size()-1;
}

////////////////////////////////////////////////////////////////
int Route::getNode(unsigned p) const
{
    assert(p >= 0 && p < Path.size());
    return Path.at(p);
}

////////////////////////////////////////////////////////////////
void Route::print() const
{
    for(unsigned int i = 0; i < Path.size(); i++)
        cout<<Path.at(i)<<"-";
}
