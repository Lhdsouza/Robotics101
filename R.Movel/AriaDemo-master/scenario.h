#ifndef SCENARIO_H
#define SCENARIO_H

#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QColor>
#include <QPixmap>

#include <iostream>
#include <math.h>

#include "robot.h"

using namespace std;

namespace Ui {
class scenario;
}

#define TILE_FREE 0
#define TILE_OBSTACLE 1
#define TILE_ROBOT 2

class Robot;

class Scenario : public QWidget
{
    Q_OBJECT
public:
    explicit Scenario(QWidget *parent = 0, unsigned int gridSize = 1);
    ~Scenario();

public slots:
    void setBlock(int x, int y, int type);
    virtual void updateDesenho(Robot *robot) = 0;

private:
    Ui::scenario *ui;
    int **board;

protected:
    unsigned int gridSize;
    unsigned int tileSize;

    void paintEvent(QPaintEvent *event);
    virtual void customTilePaint(int x, int y, int type, QPainter *p) = 0;
};

#endif // SCENARIO_H
