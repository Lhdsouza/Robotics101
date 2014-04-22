#ifndef SCENARIO_H
#define SCENARIO_H

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QPixmap>

#include "robot.h"

#include <iostream>
#include <math.h>

using namespace std;

namespace Ui {
class scenario;
}

#define TILE_FREE 0
#define TILE_OBSTACLE 1
#define TILE_ROBOT 2

class scenario : public QWidget
{
    Q_OBJECT
public:
    explicit scenario(QWidget *parent = 0, unsigned int gridSize = 1);
    ~scenario();

public slots:
    void setBlock(int x, int y, int type);
    void updateDesenho(Robot* robot);

private:
    Ui::scenario *ui;
    int **board;
    unsigned int gridSize;
    unsigned int tileSize;

protected:
    void paintEvent(QPaintEvent *event);

};

#endif // SCENARIO_H
