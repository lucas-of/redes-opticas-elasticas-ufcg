#include "tela_principal.h"
#include "ui_tela_principal.h"
#include "ajuda_form.h"

tela_principal::tela_principal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::tela_principal)
{
    ui->setupUi(this);

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

    ui->comboBox_op_alg_rote->addItem(" ");
    ui->comboBox_op_alg_rote->addItem("Minimum Hops");
    ui->comboBox_op_alg_rote->addItem("CSP");
    ui->comboBox_op_alg_rote->addItem("CSP Acumulado");
    ui->comboBox_op_alg_rote->addItem("Shortest Path");
    ui->comboBox_op_alg_rote->addItem("AWR");
    ui->comboBox_op_alg_rote->addItem("AWE com Ruído do Enlace");
    ui->comboBox_op_alg_rote->addItem("LOR Num. Formas");
    ui->comboBox_op_alg_rote->addItem("LOR Disponibilidade");
    ui->comboBox_op_alg_rote->addItem("PSO");
    ui->comboBox_op_alg_rote->addItem("OSNR-R");
    //cout << "\t" << MH<<" - Minimum Hops \n\t"<<CSP<<" - CSP\n\t"<< CSP_Acum<<" - CSP Acumulado\n\t" << SP << " - Shortest Path\n\t"<< DJK_SPeFormas << " - AWR\n\t" << DJK_RuidoEFormas << " - AWR com Ruído do Enlace\n\t" << LOR_NF << " - LOR Num. Formas\n\t" << LOR_A << " - LOR Disponibilidade\n\t" << Dij_PSO << " - PSO\n\t" << OSNRR << " - OSNR-R\n";

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

void tela_principal::on_actionSair_triggered()
{
    close();
}

void tela_principal::on_actionAjuda_triggered()
{
    ajuda_form tela_ajuda;
    tela_ajuda.setModal(true);
    tela_ajuda.exec();
}

void tela_principal::on_comboBox_op_sim_currentIndexChanged(const QString &arg1)
{
    if((ui->comboBox_op_sim->currentIndex()!=0)&&(ui->radioButton_cons_osnr_sim->isChecked()==true))
    {
        ui->label_pot_entr->setEnabled(true);
        ui->label_pot_ref->setEnabled(true);
        ui->lineEdit_pot_entr->setEnabled(true);
        ui->lineEdit_pot_ref->setEnabled(true);

        if(ui->comboBox_op_sim->currentIndex()!=3)
        {
            ui->label_dist_amp->setEnabled(true);
            ui->lineEdit_dist_amp->setEnabled(true);
        }
        else
        {
            ui->label_dist_amp->setEnabled(false);
            ui->lineEdit_dist_amp->setEnabled(false);
        }

        ui->label_arq->setEnabled(true);
        ui->radioButton_arq_bs->setEnabled(true);
        ui->radioButton_arq_ss->setEnabled(true);
    }

    if((ui->comboBox_op_sim->currentIndex()!=3)&&(ui->comboBox_op_sim->currentIndex()!=0))
    {
        ui->label_num_req->setEnabled(true);
        ui->label_num_req_cont->setEnabled(true);
        ui->lineEdit_num_req->setEnabled(true);
    }
    else
    {
        ui->label_num_req->setEnabled(false);
        ui->label_num_req_cont->setEnabled(false);
        ui->lineEdit_num_req->setEnabled(false);
    }

    if(ui->comboBox_op_sim->currentIndex()==7)
    {
        ui->label_rep_sim->setEnabled(true);
        ui->lineEdit_rep_sim->setEnabled(true);

        ui->label_otim_dist_ruid->setEnabled(false);
        ui->radioButton_otim_dist->setEnabled(false);
        ui->radioButton_otim_ruid->setEnabled(false);

        ui->label_awr_otim->setEnabled(false);
        ui->radioButton_awr_otim_nao->setEnabled(false);
        ui->radioButton_awr_otim_sim->setEnabled(false);

        ui->label_op_alg_rote->setEnabled(true);
        ui->comboBox_op_alg_rote->setEnabled(true);

        ui->label_osnr_min->setEnabled(false);
        ui->label_osnr_max->setEnabled(false);
        ui->label_pontos_graf->setEnabled(false);
        ui->lineEdit_osnr_min->setEnabled(false);
        ui->lineEdit_osnr_max->setEnabled(false);
        ui->lineEdit_pontos_graf->setEnabled(false);

        ui->label_la_max->setEnabled(false);
        ui->label_la_min->setEnabled(false);
        ui->lineEdit_la_max->setEnabled(false);
        ui->lineEdit_la_min->setEnabled(false);

        ui->label_la->setEnabled(true);
        ui->lineEdit_la->setEnabled(true);

        ui->label_dist_max_amp->setEnabled(false);
        ui->label_dist_max_amp_cont->setEnabled(false);
        ui->label_dist_min_amp->setEnabled(false);
        ui->label_dist_min_amp_cont->setEnabled(false);
        ui->lineEdit_dist_max_amp->setEnabled(false);
        ui->lineEdit_dist_min_amp->setEnabled(false);
    }
    else if(ui->comboBox_op_sim->currentIndex()==6)
    {
        ui->label_otim_dist_ruid->setEnabled(true);
        ui->radioButton_otim_dist->setEnabled(true);
        ui->radioButton_otim_ruid->setEnabled(true);

        ui->label_rep_sim->setEnabled(false);
        ui->lineEdit_rep_sim->setEnabled(false);

        ui->label_awr_otim->setEnabled(false);
        ui->radioButton_awr_otim_nao->setEnabled(false);
        ui->radioButton_awr_otim_sim->setEnabled(false);

        ui->label_op_alg_rote->setEnabled(false);
        ui->comboBox_op_alg_rote->setEnabled(false);

        if(ui->radioButton_otim_dist->isChecked()==true)
        {
            ui->comboBox_op_alg_rote->setCurrentIndex(5);
        }
        else if(ui->radioButton_otim_ruid->isChecked()==true)
        {
            ui->comboBox_op_alg_rote->setCurrentIndex(6);
        }

        ui->label_osnr_min->setEnabled(false);
        ui->label_osnr_max->setEnabled(false);
        ui->label_pontos_graf->setEnabled(false);
        ui->lineEdit_osnr_min->setEnabled(false);
        ui->lineEdit_osnr_max->setEnabled(false);
        ui->lineEdit_pontos_graf->setEnabled(false);

        ui->label_la_max->setEnabled(false);
        ui->label_la_min->setEnabled(false);
        ui->lineEdit_la_max->setEnabled(false);
        ui->lineEdit_la_min->setEnabled(false);

        ui->label_la->setEnabled(true);
        ui->lineEdit_la->setEnabled(true);

        ui->label_dist_max_amp->setEnabled(false);
        ui->label_dist_max_amp_cont->setEnabled(false);
        ui->label_dist_min_amp->setEnabled(false);
        ui->label_dist_min_amp_cont->setEnabled(false);
        ui->lineEdit_dist_max_amp->setEnabled(false);
        ui->lineEdit_dist_min_amp->setEnabled(false);
    }
    else if(ui->comboBox_op_sim->currentIndex()==5) // Ainda há outra condição, mas não entendi bem
    {
        ui->label_awr_otim->setEnabled(true);
        ui->radioButton_awr_otim_nao->setEnabled(true);
        ui->radioButton_awr_otim_sim->setEnabled(true);

        ui->label_rep_sim->setEnabled(false);
        ui->lineEdit_rep_sim->setEnabled(false);

        ui->label_otim_dist_ruid->setEnabled(false);
        ui->radioButton_otim_dist->setEnabled(false);
        ui->radioButton_otim_ruid->setEnabled(false);

        ui->label_op_alg_rote->setEnabled(false);
        ui->comboBox_op_alg_rote->setEnabled(false);

        ui->label_osnr_min->setEnabled(false);
        ui->label_osnr_max->setEnabled(false);
        ui->label_pontos_graf->setEnabled(false);
        ui->lineEdit_osnr_min->setEnabled(false);
        ui->lineEdit_osnr_max->setEnabled(false);
        ui->lineEdit_pontos_graf->setEnabled(false);

        ui->label_la_max->setEnabled(false);
        ui->label_la_min->setEnabled(false);
        ui->lineEdit_la_max->setEnabled(false);
        ui->lineEdit_la_min->setEnabled(false);

        ui->label_la->setEnabled(true);
        ui->lineEdit_la->setEnabled(true);

        ui->label_dist_max_amp->setEnabled(false);
        ui->label_dist_max_amp_cont->setEnabled(false);
        ui->label_dist_min_amp->setEnabled(false);
        ui->label_dist_min_amp_cont->setEnabled(false);
        ui->lineEdit_dist_max_amp->setEnabled(false);
        ui->lineEdit_dist_min_amp->setEnabled(false);
    }
    else if(ui->comboBox_op_sim->currentIndex()==3)
    {
        ui->label_osnr_min->setEnabled(true);
        ui->label_osnr_max->setEnabled(true);
        ui->label_pontos_graf->setEnabled(true);
        ui->lineEdit_osnr_min->setEnabled(true);
        ui->lineEdit_osnr_max->setEnabled(true);
        ui->lineEdit_pontos_graf->setEnabled(true);

        ui->label_rep_sim->setEnabled(false);
        ui->lineEdit_rep_sim->setEnabled(false);

        ui->label_otim_dist_ruid->setEnabled(false);
        ui->radioButton_otim_dist->setEnabled(false);
        ui->radioButton_otim_ruid->setEnabled(false);

        ui->label_awr_otim->setEnabled(false);
        ui->radioButton_awr_otim_nao->setEnabled(false);
        ui->radioButton_awr_otim_sim->setEnabled(false);

        ui->label_op_alg_rote->setEnabled(true);
        ui->comboBox_op_alg_rote->setEnabled(true);

        ui->label_la_max->setEnabled(false);
        ui->label_la_min->setEnabled(false);
        ui->lineEdit_la_max->setEnabled(false);
        ui->lineEdit_la_min->setEnabled(false);

        ui->label_la->setEnabled(false);
        ui->lineEdit_la->setEnabled(false);

        ui->label_dist_max_amp->setEnabled(true);
        ui->label_dist_max_amp_cont->setEnabled(true);
        ui->label_dist_min_amp->setEnabled(true);
        ui->label_dist_min_amp_cont->setEnabled(true);
        ui->lineEdit_dist_max_amp->setEnabled(true);
        ui->lineEdit_dist_min_amp->setEnabled(true);
        ui->label_pontos_graf->setEnabled(true);
        ui->lineEdit_pontos_graf->setEnabled(true);
    }
    else if((ui->comboBox_op_sim->currentIndex()==2) || (ui->comboBox_op_sim->currentIndex()==3))
    {
        ui->label_osnr_min->setEnabled(true);
        ui->label_osnr_max->setEnabled(true);
        ui->label_pontos_graf->setEnabled(true);
        ui->lineEdit_osnr_min->setEnabled(true);
        ui->lineEdit_osnr_max->setEnabled(true);
        ui->lineEdit_pontos_graf->setEnabled(true);

        ui->label_rep_sim->setEnabled(false);
        ui->lineEdit_rep_sim->setEnabled(false);

        ui->label_otim_dist_ruid->setEnabled(false);
        ui->radioButton_otim_dist->setEnabled(false);
        ui->radioButton_otim_ruid->setEnabled(false);

        ui->label_awr_otim->setEnabled(false);
        ui->radioButton_awr_otim_nao->setEnabled(false);
        ui->radioButton_awr_otim_sim->setEnabled(false);

        ui->label_op_alg_rote->setEnabled(true);
        ui->comboBox_op_alg_rote->setEnabled(true);

        ui->label_la_max->setEnabled(false);
        ui->label_la_min->setEnabled(false);
        ui->lineEdit_la_max->setEnabled(false);
        ui->lineEdit_la_min->setEnabled(false);

        ui->label_la->setEnabled(false);
        ui->lineEdit_la->setEnabled(false);

        ui->label_dist_max_amp->setEnabled(false);
        ui->label_dist_max_amp_cont->setEnabled(false);
        ui->label_dist_min_amp->setEnabled(false);
        ui->label_dist_min_amp_cont->setEnabled(false);
        ui->lineEdit_dist_max_amp->setEnabled(false);
        ui->lineEdit_dist_min_amp->setEnabled(false);
    }
    else if(ui->comboBox_op_sim->currentIndex()==1)
    {
        ui->label_la_max->setEnabled(true);
        ui->label_la_min->setEnabled(true);
        ui->label_pontos_graf->setEnabled(true);
        ui->lineEdit_la_max->setEnabled(true);
        ui->lineEdit_la_min->setEnabled(true);
        ui->lineEdit_pontos_graf->setEnabled(true);

        ui->label_osnr_min->setEnabled(false);
        ui->label_osnr_max->setEnabled(false);
        ui->lineEdit_osnr_min->setEnabled(false);
        ui->lineEdit_osnr_max->setEnabled(false);

        ui->label_la->setEnabled(false);
        ui->lineEdit_la->setEnabled(false);

        ui->label_dist_max_amp->setEnabled(false);
        ui->label_dist_max_amp_cont->setEnabled(false);
        ui->label_dist_min_amp->setEnabled(false);
        ui->label_dist_min_amp_cont->setEnabled(false);
        ui->lineEdit_dist_max_amp->setEnabled(false);
        ui->lineEdit_dist_min_amp->setEnabled(false);
    }
    else
    {
        ui->label_rep_sim->setEnabled(false);
        ui->lineEdit_rep_sim->setEnabled(false);

        ui->label_otim_dist_ruid->setEnabled(false);
        ui->radioButton_otim_dist->setEnabled(false);
        ui->radioButton_otim_ruid->setEnabled(false);

        ui->label_awr_otim->setEnabled(false);
        ui->radioButton_awr_otim_nao->setEnabled(false);
        ui->radioButton_awr_otim_sim->setEnabled(false);

        ui->label_op_alg_rote->setEnabled(true);
        ui->comboBox_op_alg_rote->setEnabled(true);

        ui->label_osnr_min->setEnabled(false);
        ui->label_osnr_max->setEnabled(false);
        ui->label_pontos_graf->setEnabled(false);
        ui->lineEdit_osnr_min->setEnabled(false);
        ui->lineEdit_osnr_max->setEnabled(false);
        ui->lineEdit_pontos_graf->setEnabled(false);

        ui->label_la_max->setEnabled(false);
        ui->label_la_min->setEnabled(false);
        ui->lineEdit_la_max->setEnabled(false);
        ui->lineEdit_la_min->setEnabled(false);

        ui->label_la->setEnabled(false);
        ui->lineEdit_la->setEnabled(false);

        ui->label_dist_max_amp->setEnabled(false);
        ui->label_dist_max_amp_cont->setEnabled(false);
        ui->label_dist_min_amp->setEnabled(false);
        ui->label_dist_min_amp_cont->setEnabled(false);
        ui->lineEdit_dist_max_amp->setEnabled(false);
        ui->lineEdit_dist_min_amp->setEnabled(false);
    }
}

void tela_principal::on_radioButton_otim_dist_clicked()
{
    if(ui->comboBox_op_sim->currentIndex()==6)
    {
        ui->label_op_alg_rote->setEnabled(false);
        ui->comboBox_op_alg_rote->setEnabled(false);
        ui->comboBox_op_alg_rote->setCurrentIndex(5);
    }
}

void tela_principal::on_radioButton_otim_ruid_clicked()
{
    if(ui->comboBox_op_sim->currentIndex()==6)
    {
        ui->label_op_alg_rote->setEnabled(false);
        ui->comboBox_op_alg_rote->setEnabled(false);
        ui->comboBox_op_alg_rote->setCurrentIndex(6);
    }
}

void tela_principal::on_comboBox_op_alg_rote_currentIndexChanged(const QString &arg1)
{
    if(ui->comboBox_op_alg_rote->currentIndex()==10)
    {
        ui->label_cons_osnr->setEnabled(false);
        ui->radioButton_cons_osnr_sim->setChecked(true);
        ui->radioButton_cons_osnr_nao->setChecked(false);
        ui->radioButton_cons_osnr_sim->setEnabled(false);
        ui->radioButton_cons_osnr_nao->setEnabled(false);
    }
    else
    {
        ui->label_cons_osnr->setEnabled(true);
        ui->radioButton_cons_osnr_sim->setEnabled(true);
        ui->radioButton_cons_osnr_nao->setEnabled(true);
    }
}

void tela_principal::on_radioButton_cons_osnr_sim_clicked()
{
    ui->label_pot_entr->setEnabled(true);
    ui->label_pot_ref->setEnabled(true);
    ui->lineEdit_pot_entr->setEnabled(true);
    ui->lineEdit_pot_ref->setEnabled(true);

    if(ui->comboBox_op_sim->currentIndex()!=3)
    {
        ui->label_dist_amp->setEnabled(true);
        ui->lineEdit_dist_amp->setEnabled(true);
    }
    else
    {
        ui->label_dist_amp->setEnabled(false);
        ui->lineEdit_dist_amp->setEnabled(false);
    }

    ui->label_arq->setEnabled(true);
    ui->radioButton_arq_bs->setEnabled(true);
    ui->radioButton_arq_ss->setEnabled(true);
}

void tela_principal::on_radioButton_cons_osnr_nao_clicked()
{
    ui->label_pot_entr->setEnabled(false);
    ui->label_pot_ref->setEnabled(false);
    ui->lineEdit_pot_entr->setEnabled(false);
    ui->lineEdit_pot_ref->setEnabled(false);

    ui->label_dist_amp->setEnabled(false);
    ui->lineEdit_dist_amp->setEnabled(false);

    ui->label_arq->setEnabled(false);
    ui->radioButton_arq_bs->setEnabled(false);
    ui->radioButton_arq_ss->setEnabled(false);
}

void tela_principal::on_radioButton_cons_exp_compr_sim_clicked()
{
    ui->label_maux_lae->setEnabled(true);
    ui->label_maux_muc->setEnabled(true);
    ui->lineEdit_maux_lae->setEnabled(true);
    ui->lineEdit_maux_muc->setEnabled(true);
}

void tela_principal::on_radioButton_cons_exp_compr_nao_clicked()
{
    ui->label_maux_lae->setEnabled(false);
    ui->label_maux_muc->setEnabled(false);
    ui->lineEdit_maux_lae->setEnabled(false);
    ui->lineEdit_maux_muc->setEnabled(false);
}
