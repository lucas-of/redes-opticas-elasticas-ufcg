#include "Def.h"

long double AvaliarOSNR(const Route *Rota, int NSlotsUsed) {
    long double Potencia = Def::get_Pin();
    long double Ruido = Def::get_Pin()/General::dB(Def::get_OSRNin());
    long double osnr;

    for (unsigned i = 0; i<= Rota->getNhops() ; i++ ) {
        if (i!=0) {
            Potencia *= Rede.at(Rota->getNode(i)).get_gain_preamp();
            Ruido *= Rede.at(Rota->getNode(i)).get_gain_preamp();
            Ruido += Rede.at(Rota->getNode(i)).get_ruido_preamp(NSlotsUsed); //Perdas nos amplificadores de potência
            //cout << "Adicionar Ruido PREAMP do Node " << Rota->getNode(i) << " valendo " << Rede.at(Rota->getNode(i)).get_ruido_preamp(NSlotsUsed) << endl;
            Potencia *= Rede.at(Rota->getNode(i)).get_loss();
            Ruido *= Rede.at(Rota->getNode(i)).get_loss(); //Perda nos elementos da rede (demux)
        }

        if (i != Rota->getNhops()) {
            Potencia *= Rede.at(Rota->getNode(i)).get_loss();
            Ruido *= Rede.at(Rota->getNode(i)).get_loss(); //Perda nos elementos da rede (mux)
            Potencia *= Rede.at(Rota->getNode(i)).get_gain_pot();
            Ruido *= Rede.at(Rota->getNode(i)).get_gain_pot();
            Ruido += Rede.at(Rota->getNode(i)).get_ruido_pot(NSlotsUsed); //Perdas nos preamplificadores
            //cout << "Adicionar Ruido POT do Node " << Rota->getNode(i) << " valendo " << Rede.at(Rota->getNode(i)).get_ruido_pot(NSlotsUsed) << endl;

            Ruido += Caminho[Rota->getNode(i)].at(Rota->getNode(i+1)).get_ruido_enlace(NSlotsUsed); //perda no enlace
            //cout << "Adicionar Ruido do ENLACE entre " << Rota->getNode(i) << " e " << Rota->getNode(i+1) << ", com " << Caminho[Rota->getNode(i)].at(Rota->getNode(i+1)).get_comprimento() << " valendo  " << Caminho[Rota->getNode(i)].at(Rota->getNode(i+1)).get_ruido_enlace(NSlotsUsed) << endl;
        }
    }

    osnr = 10*log10(Potencia/Ruido);
    cout << "osnr: " << osnr;
    return osnr;
}

void AccountForBlockingOSNR(int NslotsReq, int NslotsUsed) {
    if(NslotsUsed <= 0) //A conexao foi bloqueada
        Def::numReq_BloqPorOSNR++;
    Def::numSlots_BloqPorOSNR += (NslotsReq - NslotsUsed);
}
