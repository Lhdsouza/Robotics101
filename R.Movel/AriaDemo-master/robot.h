#ifndef ROBOT_H
#define ROBOT_H

#include <Aria.h>
#include <iostream>
#include <QObject>
#include <QThread>
#include <QTimer>
#include <list>
#include <cstddef>
#include "scenario.h"

using namespace std;

class Scenario;

class Robot: public QObject, public ArRobot
{
    Q_OBJECT

public:
    Robot(int *argc, char **argv, Scenario *scene);
    ~Robot();
    void rotate(int degrees);
    int getLaserRange(int angle);
    int getSonarRange(int id_sonar);
    int wardOff(); // Retorna a orientacao para o robo
    int searchElement(); // Retorna o Objeto mais proximo
    double getNorth();

    void exploration();
    void imStop();
    void rotateTo(int sonarFiles[]);
    void PrintMap();
    double OldPx;
    double OldPy;
    int sonars[8];
    vector<ArSensorReading> *lasers;

    Scenario *scene;

    list<double>mapX;
    list<double>mapY;

    int MapR[30][30];
    //int apontador = 0;
    void start();
private slots:
    bool initializeAria();
public slots:
    bool shutdown();
    bool maketri();
    void readingSensors();
    void move(int distanceMM);
private:
    ArSick sick;
    ArSonarDevice sonar;
    ArRobotConnector *robotConnector;
    ArLaserConnector *laserConnector;
    ArArgumentParser parser;
    //vector<ArSensorReading> *lasers = NULL;
    //int sonars[8] = {0,0,0,0,0,0,0,0};
    QThread *thread;
    QTimer *timer;

signals:
    void updateDesenho(Robot *robot);
//    void updateDesenho(int sonarFiles[], double theta, double robotX, double robotY);

};

#endif // ROBOT_H
