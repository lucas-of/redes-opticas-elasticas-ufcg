#ifndef TELA_PRINCIPAL_H
#define TELA_PRINCIPAL_H

#include <QMainWindow>

namespace Ui {
class tela_principal;
}

class tela_principal : public QMainWindow
{
    Q_OBJECT

public:
    explicit tela_principal(QWidget *parent = 0);
    ~tela_principal();

private slots:
    void on_actionSair_triggered();

    void on_actionAjuda_triggered();

    void on_comboBox_op_sim_currentIndexChanged(const QString &arg1);

    void on_radioButton_otim_dist_clicked();

    void on_radioButton_otim_ruid_clicked();

    void on_comboBox_op_alg_rote_currentIndexChanged(const QString &arg1);

    void on_radioButton_cons_osnr_sim_clicked();

    void on_radioButton_cons_osnr_nao_clicked();

    void on_radioButton_cons_exp_compr_sim_clicked();

    void on_radioButton_cons_exp_compr_nao_clicked();

private:
    Ui::tela_principal *ui;
};

#endif // TELA_PRINCIPAL_H
