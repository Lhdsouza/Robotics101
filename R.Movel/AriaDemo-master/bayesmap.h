#ifndef BAYESMAP_H
#define BAYESMAP_H

#include <QtGui>
#include "scenario.h"

class BayesMap : public Scenario
{
public:
    BayesMap(QWidget *parent, unsigned int gridSize, double occupationThreshold);

public slots:
    void updateDesenho(Robot *robot);

protected:
    void customTilePaint(int x, int y, int type, QPainter *p);

private:
    double **bayes;
    double occupationThreshold;
};

#endif // BAYESMAP_H
