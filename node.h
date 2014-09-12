#ifndef NODE_H
#define NODE_H

class Node
{
    static long double Nnz;//Representa a posicão do amplificador
    static long double Nin;//Ruido de entrada, que é dado por potencia sobre ruido de entrada
public:
    Node();
    static void calc_Nin();
    static void calc_Nnz();
};

long void Node::Nin=0;
long void Node::Nnz=0;

#endif // NODE_H

