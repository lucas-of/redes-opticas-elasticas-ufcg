#include "Route.h"
#include "Main_Auxiliar.h"

Route::Route(std::vector<Node *> &path) {
    for ( int i = 0; i < path.size(); i++ ) {
        Path.push_back(path.at(i)->get_whoami());
    }
    Nodes = path;
}

int Route::getDeN() const {
    assert(Path.size() > 0);
    return Path.back();
}

int Route::getNhops() const {
    assert(Path.size() > 0);
    return Path.size() - 1;
}

int Route::getNode(unsigned int p) const {
    assert((p >= 0) && (p < (int) Path.size()));
    return Path.at(p);
}

int Route::getOrN() const {
    assert(Path.size() > 0);
    return Path.front();
}

void Route::print() const {
    for ( int i = 0; i < (int) Path.size(); i++ )
        std::cout << Path.at(i) << "--";
}

Route Route::breakRoute(int OrN, int DeN) {
    int LocOrN, LocDeN;
    for ( int i = 0; i < Path.size(); i++ ) {
        if ( OrN == Path.at(i) ) LocOrN = i;
        if ( DeN == Path.at(i) ) LocDeN = i;
    }

    assert(LocOrN < LocDeN);

    vector <Node*> Caminho;
    for ( int i = LocOrN; i <= LocDeN; i++ )
        Caminho.push_back(Nodes.at(i));

    return Route(Caminho);
}

std::vector<Node*> *Route::get_Nodes() {
    return &Nodes;
}
