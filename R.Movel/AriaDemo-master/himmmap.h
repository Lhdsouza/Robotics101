#ifndef HIMMMAP_H
#define HIMMMAP_H

#include <QtGui>
#include "scenario.h"

class HimmMap : public Scenario
{
public:
    HimmMap(QWidget *parent, unsigned int gridSize, int minValue, int maxValue, int increment, int decrement);

public slots:
    void updateDesenho(Robot *robot);

protected:
    void customTilePaint(int x, int y, int type, QPainter *p);

private:
    int **himm;
    int minValue;
    int maxValue;
    int increment;
    int decrement;
};

#endif // HIMMMAP_H
