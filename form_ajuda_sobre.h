#ifndef FORM_AJUDA_SOBRE_H
#define FORM_AJUDA_SOBRE_H

#include <QDialog>

namespace Ui {
class form_ajuda_sobre;
}

class form_ajuda_sobre : public QDialog
{
    Q_OBJECT

public:
    explicit form_ajuda_sobre(QWidget *parent = 0);
    ~form_ajuda_sobre();

private:
    Ui::form_ajuda_sobre *ui;
};

#endif // FORM_AJUDA_SOBRE_H
