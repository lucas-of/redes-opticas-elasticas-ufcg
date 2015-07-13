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
    void on_actionSobre_triggered();

    void on_actionSair_triggered();

    void on_pushButton_sobre_clicked();

    void on_pushButton_nova_sim_clicked();

    void on_pushButton_proximo_clicked();

    void on_actionNova_Simula_o_triggered();

private:
    Ui::tela_principal *ui;
};

#endif // TELA_PRINCIPAL_H
