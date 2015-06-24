#include "Node.h"
#include "General.h"
#include "Def.h"
#include "Constantes.h"
#include "Regeneradores.h"

Node::Node(int who) {
    assert(who >= 0);
    whoami = who;
    loss = General::dB(-1 * Def::get_Lsss());
    TipoNo = Transparente;

    NumRegeneradores = 0;
    NumRegeneradoresDisponiveis = 0;
}

int Node::get_whoami() {
    return whoami;
}

void Node::calcula_ganho_pot(Def *Config) {
    gain_pot = General::dB(General::lin(Config->get_Pref() / 1E-3L) - potencia_tx + Config->get_Lsss());
    calcula_ruido_pot();
}

void Node::calcula_ruido_pot() {
    long double freq = Def::get_freq();
    ruido_pot = Def::get_Famp()*(gain_pot - 1.0) * Constante::h * freq * Def::get_Bref();
}

long double Node::get_loss() {
    return loss;
}

long double Node::get_gain_pot(Def *Config) {
    calcula_ganho_pot(Config);
    return gain_pot;
}

long double Node::get_ruido_pot(Def *Config) {
    calcula_ganho_pot(Config);
    return ruido_pot;
}

void Node::set_potenciatx(long double potencia) {
    //Potencia em dBm
    potencia_tx = potencia;

}

void Node::set_TipoNo(TiposDeNo Tipo) {
    TipoNo = Tipo;
}

Node::TiposDeNo Node::get_TipoNo() {
    return TipoNo;
}

void Node::set_NumRegeneradores(int NR) {
    assert(NR > 0);
    assert(TipoNo == Translucido);

    NumRegeneradores = NR;
    NumRegeneradoresDisponiveis = NR;
}

int Node::get_NumRegeneradores() {
    return NumRegeneradores;
}

int Node::get_NumRegeneradoresDisponiveis() {
    return NumRegeneradoresDisponiveis;
}

int Node::solicitar_regeneradores(int Taxa) {
    assert(TipoNo == Translucido);
    int NumReg = ceil(Taxa / Regeneradores::BR);
    if ( NumRegeneradoresDisponiveis >= NumReg ) {
        NumRegeneradoresDisponiveis -= NumReg;
        return NumReg;
    } else
        return 0;
}

void Node::liberar_regeneradores(int NumReg) {
    assert(TipoNo == Translucido);
    assert(NumRegeneradores - NumRegeneradoresDisponiveis >= NumReg);

    NumRegeneradoresDisponiveis += NumReg;
}
