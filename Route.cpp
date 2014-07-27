#include "Route.h"

Route::Route(std::vector<int>& path) : Path(path) {}

int Route::getDeN() const {
    assert(Path.size()>0);
    return Path.back();
}

unsigned int Route::getNhops() const {
    assert(Path.size()>0);
    return Path.size()-1;
}

int Route::getNode(int p) const {
    assert((p >= 0) && (p < (int) Path.size()));
    return Path.at(p);
}

int Route::getOrN() const {
    assert(Path.size()>0);
    return Path.front();
}

void Route::print() const {
    for (int i = 0; i < (int) Path.size(); i++)
        std::cout << Path.at(i) << "--";
}
