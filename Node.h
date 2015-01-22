#ifndef NODE_H
#define NODE_H

class Node {
    private:
        int whoami;
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
        int get_whoami();
        long double get_loss();
        long double get_gain_preamp();
        long double get_ruido_preamp(int);
        long double get_gain_pot();
        long double get_ruido_pot(int);
};

#endif //NODE_H
