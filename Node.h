#ifndef NODE_H
#define NODE_H

class Node {
    private:
        int whoami;
        long double Nnz;//Representa a posicão do amplificador
        long double Nin;//Ruido de entrada, que é dado por potencia sobre ruido de entrada
        long double loss;
        long double gain;
        long double noise;
        void calcula_ganho();
        void calcula_ruido();
    public:
        Node(int);
        void calc_Nin();
        int get_whoami();
        long double get_loss();
        long double get_gain();
        long double get_noise();
};

#endif //NODE_H
