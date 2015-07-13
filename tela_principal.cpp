#include "tela_principal.h"
#include "ui_tela_principal.h"
#include "form_ajuda_sobre.h"

tela_principal::tela_principal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tela_principal)
{
    ui->setupUi(this);

    ui->stackedWidget_principal->setCurrentIndex(0);

    ui->comboBox_op_sim->addItem(" ");
    ui->comboBox_op_sim->addItem("Probabilidade de Bloqueio");
    ui->comboBox_op_sim->addItem("OSNR");
    ui->comboBox_op_sim->addItem("Distância dos Amplificadores");
    ui->comboBox_op_sim->addItem("Número de Slots");
    ui->comboBox_op_sim->addItem("PSR - Otimização");
    ui->comboBox_op_sim->addItem("Otimização do Alfa/Beta");
    ui->comboBox_op_sim->addItem("Bigode");

    //cout << "Escolha a Simulação. " << endl << "\tProbabilidade de Bloqueio <" << Sim_PbReq << ">;" << endl << "\tOSNR <" << Sim_OSNR << ">; " << endl << "\tDistancia dos Amplificadores <" << Sim_DAmp << ">;" << endl << "\tNumero de Slots <" << Sim_NSlots << ">;" << endl << "\tPSR - Otimização <" << Sim_TreinoPSR << ">;" << endl << "\tOtimização do Alfa/Beta <" << Sim_AlfaBetaOtimizado << ">;" << endl << "\tBigode <" << Sim_Bigode << ">." << endl;

    ui->comboBox_op_topo->addItem(" ");
    ui->comboBox_op_topo->addItem("Pacific Bell");
    ui->comboBox_op_topo->addItem("NSFNet");
    ui->comboBox_op_topo->addItem("NSFNet Modificada (Reduzida)");
    ui->comboBox_op_topo->addItem("Ponto a Ponto de 4 Nós");
    ui->comboBox_op_topo->addItem("Ponto a Ponto de 8 Nós");
    ui->comboBox_op_topo->addItem("Top 1");
    ui->comboBox_op_topo->addItem("Top 2");

    //cout << "Escolha a topologia." << endl << "\tPacific Bell <" << PacificBell << ">; "<< endl << "\tNSFNet <" << NSFNet << ">; " << endl << "\tNSFNet Modificada (Reduzida) <" << NFSNetMod << ">;" << endl << "\tPonto a Ponto de 4 Nós <" << PontoaPonto4 <<">; "  << endl << "\tPonto a Ponto de 8 Nós <" << PontoaPonto8 << ">; " << endl << "\tTop1 <" << Top1 << ">;" << endl << "\tTop2 <" << Top2 << ">;" << endl;

    ui->comboBox_op_alg_aloc->addItem(" ");
    ui->comboBox_op_alg_aloc->addItem("Random");
    ui->comboBox_op_alg_aloc->addItem("First Fit");
    ui->comboBox_op_alg_aloc->addItem("Most Used");
    ui->comboBox_op_alg_aloc->addItem("First Fit Opt");
    //cout<<"\t" << RD<<" - Random \n\t"<<FF<<" - FirstFit \n\t"<<MU<<" - Most Used \n\t"<<FFO<<" - FirstFitOpt "<<endl;
}

tela_principal::~tela_principal()
{
    delete ui;
}

void tela_principal::on_actionSobre_triggered()
{
    form_ajuda_sobre tela_ajuda_sobre;
    tela_ajuda_sobre.setModal(true);
    tela_ajuda_sobre.exec();
}

void tela_principal::on_actionSair_triggered()
{
    close();
}

void tela_principal::on_pushButton_sobre_clicked()
{
    form_ajuda_sobre tela_ajuda_sobre;
    tela_ajuda_sobre.setModal(true);
    tela_ajuda_sobre.exec();
}

void tela_principal::on_pushButton_nova_sim_clicked()
{
    ui->stackedWidget_principal->setCurrentIndex(1); //Pg. 2 do SW Principal
}

void tela_principal::on_pushButton_proximo_clicked()
{

}

void tela_principal::on_actionNova_Simula_o_triggered()
{
    ui->stackedWidget_principal->setCurrentIndex(1); //Pg. 2 do SW Principal
}
