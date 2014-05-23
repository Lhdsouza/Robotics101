#include "scenario.h"
#include "ui_scenario.h"

#include <QtGui>


Scenario::Scenario(QWidget *parent, unsigned int gridSize) :
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

void Scenario::paintEvent(QPaintEvent *){
    QPainter painter(this);

    for(unsigned int xI = 0; xI < gridSize; xI++) {
        for(unsigned int yI = 0; yI < gridSize; yI++) {

            // Do custom modifications.
            customTilePaint(xI, yI, board[xI][yI], &painter);

            // Choose brush according to type.
            painter.setOpacity(1.0);
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


void Scenario::setBlock(int x, int y, int type){
    this->board[x][y] = type;
}


Scenario::~Scenario()
{
    delete ui;
    for(int i; i < gridSize; i++)
        delete this->board[i];
    delete [] this->board;
}
