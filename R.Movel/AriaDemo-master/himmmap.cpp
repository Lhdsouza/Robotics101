#include "himmmap.h"

HimmMap::HimmMap(QWidget *parent, unsigned int gridSize, int minValue, int maxValue, int increment, int decrement):
    Scenario(parent, gridSize),
    minValue(minValue),
    maxValue(maxValue),
    increment(increment),
    decrement(decrement)
{
    // Allocate bayes values matrix.
    himm = new int*[gridSize];
    for(int i = 0; i < gridSize; i++) {
        himm[i] = new int[gridSize];
    }

    // Set all values as 0.5.
    for(int i = 0; i < gridSize; i++) {
        for(int j = 0; j < gridSize; j++) {
            this->himm[i][j] = 0;
        }
    }
}

void HimmMap::updateDesenho(Robot *robot)
{
    int n = 22000/gridSize;
    double fixedAdvance = n / 2.0;
    double theta = robot->getNorth(), robotX = robot->getX(), robotY = robot->getY();
    int meXGlobal = (gridSize/2) + (robotX /n);
    int meYGlobal = (gridSize/2) - (robotY /n);
    setBlock(meXGlobal, meYGlobal, TILE_ROBOT);


    for(int i=0;i<=180;i++){
        if( robot->lasers->at(i).getRange() < 4000){
            double hP =  (double) robot->lasers->at(i).getRange(); //H
            double rAngle = i - robot->getTh();
            double fixedAdvance = n / 2.0;

            for(double d = hP; d > 0; d -= fixedAdvance){
                // H² = A² + B²
                // Angulo do Laser+ Variação de theta + Angulação do Robo
                double Co = sin((M_PI*rAngle)/180.0)*d; // A - representa a variacao em Y
                double Ca = cos((M_PI*rAngle)/180.0)*d; //B - representa a variacao em X
                int Xn = meXGlobal + Co/n; // sin
                int Yn = meYGlobal - Ca/n; // cos
                if( (Yn >= 0) && (Xn >= 0) && (Xn < gridSize) && (Yn < gridSize) ) {
                    if(d==hP){
                        if(himm[Xn][Yn] >= maxValue) {
                            setBlock(Xn, Yn, TILE_OBSTACLE);
                            himm[Xn][Yn] = maxValue;
                        } else {
                            himm[Xn][Yn] += increment;
                        }
                    }
                    else if( !((Xn == meXGlobal) && (Yn == meYGlobal)) ){
                        if(himm[Xn][Yn] <= minValue) {
                            setBlock(Xn, Yn, TILE_FREE);
                            himm[Xn][Yn] = minValue;
                        } else {
                            himm[Xn][Yn] -= decrement;
                        }
                    }
                }
            }
        }
    }

    this->repaint();
}


void HimmMap::customTilePaint(int x, int y, int type, QPainter *p)
{
    p->setPen(Qt::green);
    p->setFont(QFont("Comic Sans MS", 6, QFont::Bold));
    p->drawText(tileSize*(x+0.5), tileSize*(y+1),QString::number(himm[x][y]));
}
