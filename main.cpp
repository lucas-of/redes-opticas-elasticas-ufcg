#include "tela_principal.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    tela_principal w;
    w.showMaximized();

    return a.exec();
}
