#include "form_ajuda_sobre.h"
#include "ui_form_ajuda_sobre.h"

form_ajuda_sobre::form_ajuda_sobre(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::form_ajuda_sobre)
{
    ui->setupUi(this);
}

form_ajuda_sobre::~form_ajuda_sobre()
{
    delete ui;
}
