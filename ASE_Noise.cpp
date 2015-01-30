#include "Def.h"
#include "Conexao.h"

long double AvaliarOSNR(const Route *Rota, int NSlotsUsed) {
    long double Potencia = Def::get_Pin();
    long double Ruido = Def::get_Pin()/General::dB(Def::get_OSRNin());
    long double osnr;

    for (unsigned i = 0; i<= Rota->getNhops() ; i++ ) {
        if (i!=0) {
            Potencia *= Caminho[Rota->getNode(i-1)].at(Rota->getNode(i)).get_ganho_preamplif();
            Ruido *= Caminho[Rota->getNode(i-1)].at(Rota->getNode(i)).get_ganho_preamplif();
            Ruido += Caminho[Rota->getNode(i-1)].at(Rota->getNode(i)).get_ruido_preamplif(); //Perdas nos amplificadores de potência
            Potencia *= Rede.at(Rota->getNode(i)).get_loss();
            Ruido *= Rede.at(Rota->getNode(i)).get_loss(); //Perda nos elementos da rede (demux)
        }

        if (i != Rota->getNhops()) {
            Potencia *= Rede.at(Rota->getNode(i)).get_loss();
            Ruido *= Rede.at(Rota->getNode(i)).get_loss(); //Perda nos elementos da rede (mux)
            if (i != 0) Rede.at(Rota->getNode(i)).set_potenciatx(General::lin(Def::get_Pref()/1E-3L));
            else Rede.at(Rota->getNode(i)).set_potenciatx(General::lin(Def::get_Pin()/1E-3L));
            Potencia *= Rede.at(Rota->getNode(i)).get_gain_pot();
            Ruido *= Rede.at(Rota->getNode(i)).get_gain_pot();
            Ruido += Rede.at(Rota->getNode(i)).get_ruido_pot(NSlotsUsed); //Perdas nos preamplificadores


            Potencia /= Caminho[Rota->getNode(i)].at(Rota->getNode(i+1)).get_perda_enlace();
            Ruido /= Caminho[Rota->getNode(i)].at(Rota->getNode(i+1)).get_perda_enlace();
            Potencia *= Caminho[Rota->getNode(i)].at(Rota->getNode(i+1)).get_ganho_enlace();
            Ruido *= Caminho[Rota->getNode(i)].at(Rota->getNode(i+1)).get_ganho_enlace();
            Ruido += Caminho[Rota->getNode(i)].at(Rota->getNode(i+1)).get_ruido_enlace(NSlotsUsed); //perda no enlace

        }
    }

    osnr = 10*log10(Potencia/Ruido);
    return osnr;
}

void AccountForBlockingOSNR(int NslotsReq, int NslotsUsed) {
    if(NslotsUsed <= 0) //A conexao foi bloqueada
        Def::numReq_BloqPorOSNR++;
    Def::numSlots_BloqPorOSNR += (NslotsReq - NslotsUsed);
}
