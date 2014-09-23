#ifndef NODE_H
#define NODE_H

class Node {
    private:
        int whoami;
        long double Nnz;//Representa a posicão do amplificador
        long double Nin;//Ruido de entrada, que é dado por potencia sobre ruido de entrada
        long double loss;
        long double gain_preamp;
        long double ruido_preamp;
        long double gain_pot;
        long double ruido_pot;
        void calcula_ganho_preamp();
        void calcula_ruido_preamp();
        void calcula_ganho_pot();
        void calcula_ruido_pot();
    public:
        Node(int);
        void calc_Nin();
        int get_whoami();
        long double get_loss();
        long double get_gain_preamp();
        long double get_ruido_preamp();
        long double get_gain_pot();
        long double get_ruido_pot();
};

#endif //NODE_H
