#include <QApplication>
#include "mainwindow.h"
#include "tabuleiro.h"
#include "scenario.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
