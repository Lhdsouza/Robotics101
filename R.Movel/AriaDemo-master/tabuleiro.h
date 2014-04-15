#ifndef TABULEIRO_H
#define TABULEIRO_H

#include <QMainWindow>
#include <QButtonGroup>
#include <iostream>
#include <math.h>

using::std::cout;
using::std::endl;

namespace Ui {
    class Tabuleiro;
}

class Robot;
class tabuleiro : public QMainWindow
{
    Q_OBJECT
public:
    explicit tabuleiro(QWidget *parent = 0);
    QButtonGroup Buttons;
    
signals:
    
public slots:
    void colorBlock(int code, int tpe);
    void GenerateButtons();
    void updateDesenho(Robot* robot);
//    void updateDesenho(int sonarFiles[], double theta, double robotX, double robotY);

private:
    Ui::Tabuleiro *ui;
};

#include <robot.h>

#endif // TABULEIRO_H
