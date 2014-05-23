#include "bayesmap.h"
#include <iostream>

using namespace std;

BayesMap::BayesMap(QWidget *parent, unsigned int gridSize, double occupationThreshold):
    Scenario(parent, gridSize),
    occupationThreshold(occupationThreshold)
{
    // Allocate bayes values matrix.
    bayes = new double*[gridSize];
    for(int i = 0; i < gridSize; i++) {
        bayes[i] = new double[gridSize];
    }

    // Set all values as 0.5.
    for(int i = 0; i < gridSize; i++) {
        for(int j = 0; j < gridSize; j++) {
            this->bayes[i][j] = 0.5;
        }
    }

}

void BayesMap::updateDesenho(Robot *robot)
{
    int n = 22000/gridSize;
    double fixedAdvance = n / 2.0;
    double theta = robot->getNorth(), robotX = robot->getX(), robotY = robot->getY();
    int meXGlobal = (gridSize/2) + (robotX /n);
    int meYGlobal = (gridSize/2) - (robotY /n);

    for (int i = 0; i < 8; i++) {
        int angulo = theta;
        if(i==0){angulo = 90;}
        if(i==1){angulo = 50;}
        if(i==2){angulo = 30;}
        if(i==3){angulo = 10;}
        if(i==4){angulo = -10;}
        if(i==5){angulo = -30;}
        if(i==6){angulo = -50;}
        if(i==7){angulo = -90;}
        double gap = n*1;
        if( robot->sonars[i] < 4000){
            double Hp =  robot->sonars[i]; //H
            int anguloV=30;
            cout<<"Id:"<< i <<" :-----------------------------------------"<<endl;

            for( double thet01=-(anguloV/2); thet01<=(anguloV/2);thet01++){


                //Regiao I
                for(double d=Hp-gap;d<Hp+gap;d+=fixedAdvance){
                    // H² = A² + B²
                    double nAngulo = angulo +thet01+ robot->getTh();// Angulo do Laser+ Variação de theta + Angulação do Robo
                    double Co = sin((M_PI*nAngulo)/180.0)*d; // A - representa a variacao em Y
                    double Ca = cos((M_PI*nAngulo)/180.0)*d; //B - representa a variacao em X
                    int Xn = meXGlobal + Ca/n ;
                    int Yn = meYGlobal - Co/n;

                    if( (Yn >= 0) && (Xn >= 0) && (Xn < gridSize) && (Yn < gridSize) ) {
                        double isblock = 0;
                        if(thet01 < 0){
                            thet01 = -thet01;
                            // raio -15º à 0º
                        }

                        isblock = 0.5*((((Hp-d)/Hp))+(((anguloV/2)+thet01)/(anguloV/2)));

                        this->bayes[Xn][Yn]= (isblock * this->bayes[Xn][Yn]) / ( (isblock * this->bayes[Xn][Yn]) + ( (1-isblock) * (1- this->bayes[Xn][Yn])));

                        //cout << "X:" << Xn << " Y:" << Yn << "FullProbability=" << isblock << endl;
                        if(this->bayes[Xn][Yn] > 0.95){
                            setBlock(Xn, Yn, TILE_OBSTACLE);
                        } else {
                            setBlock(Xn, Yn, TILE_FREE);
                        }


                    }
                }

                //Regiao II
                for(double d=0;d<Hp-gap;d+=fixedAdvance){
                    // H² = A² + B²
                    double nAngulo = angulo +thet01+ robot->getTh();// Angulo do Laser+ Variação de theta + Angulação do Robo
                    double Co = sin((M_PI*nAngulo)/180.0)*d; // A - representa a variacao em Y
                    double Ca = cos((M_PI*nAngulo)/180.0)*d; //B - representa a variacao em X
                    int Xn = meXGlobal + Ca/n ;
                    int Yn = meYGlobal - Co/n;

                    if( (Yn >= 0) && (Xn >= 0) && (Xn < gridSize) && (Yn < gridSize) ) {
                        double isblock = 0;
                        if(thet01 < 0){
                            thet01 = -thet01;
                        }

                        isblock = 1. - 0.5*((((Hp-d)/Hp))+(((anguloV/2)+thet01)/(anguloV/2)));


                        //cout << "Atualizando " << Xn << " " << Yn << " " << thet01 << endl
                        this->bayes[Xn][Yn]= (isblock * this->bayes[Xn][Yn]) / ( (isblock * this->bayes[Xn][Yn]) + ( (1-isblock) * (1- this->bayes[Xn][Yn])));

                        //cout << "X:" << Xn << " Y:" << Yn << "FullProbability=" << isblock << endl;
                        if(this->bayes[Xn][Yn] > 0.95){
                            setBlock(Xn, Yn, TILE_OBSTACLE);
                        } else {
                            setBlock(Xn, Yn, TILE_FREE);
                        }


                    }
                }
            }
        }
    }

    setBlock(meXGlobal, meYGlobal, TILE_ROBOT);
    this->repaint();
}


void BayesMap::customTilePaint(int x, int y, int type, QPainter *p)
{
    //    p->setPen(Qt::green);
    //    p->setFont(QFont("Comic Sans MS", 6, QFont::Bold));
    //    p->drawText(tileSize*(x+0.5), tileSize*(y+1),QString::number(bayes[x][y]));
    p->setPen(Qt::gray);
    p->setOpacity(bayes[x][y]);
    QRect rect(tileSize*x, tileSize*y, tileSize, tileSize);
    p->drawRect(rect);
}
