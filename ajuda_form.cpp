#include "ajuda_form.h"
#include "ui_ajuda_form.h"

ajuda_form::ajuda_form(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ajuda_form)
{
    ui->setupUi(this);
}

ajuda_form::~ajuda_form()
{
    delete ui;
}
