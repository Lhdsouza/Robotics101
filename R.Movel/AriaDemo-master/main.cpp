#include <QApplication>
#include "mainwindow.h"
#include "tabuleiro.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    /*
    for(int x =0; x<100;x++){
        for(int y=0;y<100;y++){
            int code = 1000*x+y;
            t.colorBlock(code);
        }
    }
    */


    return a.exec();
}
