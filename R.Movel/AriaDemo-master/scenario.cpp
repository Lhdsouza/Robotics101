#include "scenario.h"
#include "ui_scenario.h"

#include <QtGui>


scenario::scenario(QWidget *parent, unsigned int gridSize) :
    QWidget(parent),
    ui(new Ui::scenario),
    gridSize(gridSize)
{
    ui->setupUi(this);

    // Allocate board.
    this->board = new int*[gridSize];
    for(int i = 0; i < gridSize; i++)
        this->board[i] = new int[gridSize];

    // Initialize board as free tiles.
    for(int i = 0; i < gridSize; i++) {
        for(int j = 0; j < gridSize; j++) {
            this->board[i][j] = TILE_FREE;
        }
    }

    tileSize = 10;
}

void scenario::paintEvent(QPaintEvent *){
    QPainter painter(this);

    for(unsigned int xI = 0; xI < gridSize; xI++) {
        for(unsigned int yI = 0; yI < gridSize; yI++) {

            // Choose brush according to type.
            switch(board[xI][yI]) {
                case TILE_OBSTACLE:
                    painter.setBrush(Qt::black);
                    break;
                case TILE_ROBOT:
                    painter.setBrush(Qt::red);
                    break;
                case TILE_FREE:
                    painter.setBrush(Qt::NoBrush);
                    break;
            }

            // Draw board tile.
            QRect rect(tileSize*xI, tileSize*yI, tileSize, tileSize);
            painter.drawRect(rect);

        }
    }
}


void scenario::setBlock(int x, int y, int type){
    this->board[x][y] = type;
}


void scenario::updateDesenho(Robot *robot) {
    double theta = robot->getNorth(), robotX = robot->getX(), robotY = robot->getY();
    int n = 22000/gridSize;
    int xSonar[8];
    int ySonar[8];

    int meXGlobal = (gridSize/2) + (robotX /n); //Coordenadas de tabuleiro!
    int meYGlobal = (gridSize/2) - (robotY /n);
    setBlock(meXGlobal, meYGlobal, TILE_ROBOT);

    for (int i = 0; i < 8; i++) {
        xSonar[i]=0;
        ySonar[i]=0;
    }

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
        cout << "Angulo: " << angulo <<"\t Range: " << robot->sonars[i]  << endl;
        if( robot->sonars[i] < 5000){

            // H² = A² + B²
            double Hp =  robot->sonars[i]; //H
            double Co = sin((M_PI*(angulo+theta))/180.0)*Hp; // A - representa a variacao em Y
            double Ca = cos((M_PI*(angulo+theta))/180.0)*Hp; //B - representa a variacao em X


            int Xn = meXGlobal + Ca/n ;
            int Yn = meYGlobal - Co/n;

            xSonar[i] =Xn;
            ySonar[i] =Yn;



            if( (Yn >= 0) && (Xn >= 0) && (Xn < gridSize) && (Yn < gridSize) ) {
                setBlock(Xn, Yn, TILE_OBSTACLE);
            } else {
                cout << "OMG " << Xn << " " << Yn << endl;
            }
       }
    }
    this->repaint();
}

scenario::~scenario()
{
    delete ui;
    for(int i; i < gridSize; i++)
        delete this->board[i];
    delete [] this->board;
}
