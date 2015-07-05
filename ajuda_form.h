#ifndef AJUDA_FORM_H
#define AJUDA_FORM_H

#include <QDialog>

namespace Ui {
class ajuda_form;
}

class ajuda_form : public QDialog
{
    Q_OBJECT

public:
    explicit ajuda_form(QWidget *parent = 0);
    ~ajuda_form();

private:
    Ui::ajuda_form *ui;
};

#endif // AJUDA_FORM_H
