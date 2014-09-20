#ifndef NODE_H
#define NODE_H

class Node {
    private:
        int whoami;
        long double Nnz;//Representa a posicão do amplificador
        long double Nin;//Ruido de entrada, que é dado por potencia sobre ruido de entrada
    public:
        Node(int);
        void calc_Nin();
        int get_whoami();
};

#endif //NODE_H
