#ifndef NODE_H
#define NODE_H

class Node {
    private:
        long double Nnz;//Representa a posicão do amplificador
        long double Nin;//Ruido de entrada, que é dado por potencia sobre ruido de entrada
    public:
        Node();
        void calc_Nin();
        void calc_Nnz();
};

#endif // NODE_H

