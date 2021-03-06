#include "robot.h"
#include <list>
#include <iostream>
#include <math.h>

using::std::cout;
using::std::endl;




Robot::Robot(int *argc, char **argv, Scenario *scene):
    QObject(),
    ArRobot(),
    parser(argc,argv),
    scene(scene)
{
    robotConnector = new ArRobotConnector(&parser,this);

    thread = new QThread();
    this->moveToThread(thread);
    connect(thread,SIGNAL(started()),this,SLOT(initializeAria()));
    connect(thread,SIGNAL(finished()),this,SLOT(shutdown()));
    timer = new QTimer();
    timer->setInterval(500);
    connect(timer,SIGNAL(timeout()),this,SLOT(readingSensors()));
}

Robot::~Robot()
{
    this->shutdown();
}

void Robot::start()
{
    thread->start();
}

bool Robot::initializeAria()
{
    Aria::init();
    //parser.addDefaultArgument("-rp /dev/ttyUSB0 -laserPortType serial -lp /dev/ttyUSB1 ");
    //parser.addDefaultArgument("-rh 192.168.1.11 -remoteLaserTcpPort 10002");

    parser.addDefaultArgument("-rh 127.0.0.1 -remoteLaserTcpPort 10002");


    if(!robotConnector->connectRobot())
    {
        ArLog::log(ArLog::Terse,"Ops... falha ao conectar ao servidor do robo.");
        this->shutdown();
        return false;
    }

    cout << "AQUI!!!" << endl;

    ArLog::log(ArLog::Normal,"Robot connected");
    this->addRangeDevice(&sick);
    laserConnector = new ArLaserConnector(&parser,this,robotConnector);
    laserConnector->setupLaser(&sick);

    if(!laserConnector->connectLaser(&sick))
    {
        ArLog::log(ArLog::Terse,"Ops... falha ao conectar os lasers do robo.");
        this->shutdown();
        return false;
    }
    list<ArRangeDevice *> *lista = this->getRangeDeviceList();
    for(list<ArRangeDevice *>::iterator it = lista->begin(); it != lista->end();it++)
    {
        cout << "Sensor: " << (*it)->getName() << endl;
    }
    ArLog::log(ArLog::Normal,"Laser connected");
    sick.runAsync();
    this->runAsync(true);
    ArUtil::sleep(500);
    this->lock();
    this->enableMotors();
    this->unlock();
    ArUtil::sleep(500);


    //this->board.GenerateButtons();
    //this->board.show();



    timer->start();

   // this->setVel2(200,200);

    for (int l = 0; l < 30; l++) {
        for (int c = 0; c < 30; c++) {
            this->MapR[l][c]=0;
        }
    }

    int contloc =0;


    connect(this,SIGNAL(updateDesenho(Robot*)),this->scene,SLOT(updateDesenho(Robot*)));


    //board.show();
    while (true) {
       // this->setVel2(200,200);
        ArUtil::sleep(400);
        cout<<contloc<<":---------------------------------------------"<<endl;
        contloc++;
        this->exploration();
        // ArUtil::sleep(300);
        ArUtil::sleep(400);
        //this->setVel2(200,200);
    }


    return true;

}

void Robot::exploration(){

    this->OldPx = 0.0;
    this->OldPy = 0.0;
    while(true){
        //Robot::rotate(10);
        this->setVel2(0,0);
        ArUtil::sleep(400);

//        int sonardist[8];
//        for(int i = 0;i<8;i++)
//        {
//            sonardist[i]= ArRobot::getSonarRange(i);
//        }
        this->readingSensors();
        cout << "Desenha!!!! " << endl;

        emit updateDesenho(this);
//        emit updateDesenho(sonardist,this->getNorth(),this->getX(),this->getY());
         //this->rotateTo(sonardist);
         ArUtil::sleep(500);
//         this->WorldMap(sonardist);

         //this->PrintMap();
    }



}


void Robot::imStop(){
    int Xn = ArRobot::getX();
    int Yn = ArRobot::getY();

    if(Xn == this->OldPx && Yn == this->OldPy){
        ArRobot::move(-1000);
        ArUtil::sleep(500);
        Robot::rotate(180);
        ArUtil::sleep(300);
        //this->setVel2(200,200);
        ArUtil::sleep(100);
    }

    this->OldPx =ArRobot::getX();
    this->OldPy =ArRobot::getY();
}

void Robot::rotateTo(int sonarFiles[]){

    cout<<"Rotanciando ... :";
    int contElem =0;
    int Theta =0;
    int Status=0; // Número de identificação dos sonares

    for (int i = 0; i < 8; i++) {
        cout<<sonarFiles[i]<<" ";
    }
    cout<<endl;
    for (int i = 0; i < 8; i++) {
        if( sonarFiles[i] < 5000){
            if(i==0){Theta += 90;contElem++;Status+=1;}           // 0= -90º
            if(i==1){Theta += 50;contElem++;Status+=10;}          // 1= -50º
            if(i==2){Theta += 30;contElem++;Status+=100;}         // 2= -30º
            if(i==3){Theta += 10;contElem++;Status+=1000;}        // 3= -10º
            if(i==4){Theta += -10;contElem++ ;Status+=10000;}       // 4= +10º
            if(i==5){Theta += -30;contElem++ ;Status+=100000;}      // 5= +30º
            if(i==6){Theta += -50;contElem++ ;Status+=1000000;}     // 6= +50º
            if(i==7){Theta += -90;contElem++ ;Status+=10000000;}    // 7= +90º
        }
    }

    cout<<"Theta: "<< Theta<<" Elementos: "<< contElem<<endl;

    if(Theta == 0 && contElem>0){
        cout<<"Status: "<< Status<<endl;
        switch(Status){
        // todos sensores estão ativos:*******
        case 11111111:
            this->imStop();
            //this->setVel2(0,0);
            //ArUtil::sleep(300);
            //ArRobot::move(-200);
            //ArUtil::sleep(500);
            //Robot::rotate(180);
            //ArUtil::sleep(300);
            //this->setVel2(200,200);
            //ArUtil::sleep(100);
        break;

        // Livre: 0******7
        case 1111110:
            //this->setVel2(0,0);
            ArUtil::sleep(300);
            Robot::rotate(90); //-90
            ArUtil::sleep(300);
            this->setVel2(200,200);
            ArUtil::sleep(100);
        break;

        // Livre: *1****6*
        case 10111101:
            //this->setVel2(0,0);
            ArUtil::sleep(300);
            Robot::rotate(50);//-50
            ArUtil::sleep(300);
            this->setVel2(200,200);
            ArUtil::sleep(100);
            break;
        // Livre: **2**5**
        case 11011011:
            //this->setVel2(0,0);
            ArUtil::sleep(300);
            Robot::rotate(30);//-30
            ArUtil::sleep(300);
            this->setVel2(200,200);
            ArUtil::sleep(100);
            break;
        // Livre: 01****67
        case 111100:
            //this->setVel2(0,0);
            ArUtil::sleep(300);
            Robot::rotate(60);//-(30+90)/2
            ArUtil::sleep(300);
            this->setVel2(200,200);
            ArUtil::sleep(100);
            break;
        // Livre: *12**56*
        case 10011001:
            //this->setVel2(0,0);
            ArUtil::sleep(300);
            Robot::rotate(40);//-(50+30)/2
            ArUtil::sleep(300);
            this->setVel2(200,200);
            ArUtil::sleep(100);
            break;
        // Livre: 0*2**5*7
        case 01011010:
            //this->setVel2(0,0);
            ArUtil::sleep(300);
            Robot::rotate(90);//-(90)
            ArUtil::sleep(300);
            this->setVel2(200,200);
            ArUtil::sleep(100);
            break;
        // Livre: 012**567
        case 11000:
            //this->setVel2(0,0);
            ArUtil::sleep(300);
            Robot::rotate(65);//-(90+50+30)
            ArUtil::sleep(300);
            this->setVel2(200,200);
            ArUtil::sleep(100);
            break;

        default:
           // this->setVel2(0,0);
            //ArUtil::sleep(300);
            //ArRobot::move(-100);
            ArUtil::sleep(300);
            //Robot::rotate(180);
            //ArUtil::sleep(300);
            this->setVel2(200,200);
            ArUtil::sleep(100);
            break;
        }

    }

    if(contElem!=0){
       cout<<"Caso Contrario rotacionar: "<< Theta/contElem<<endl;
       this->imStop();
       Robot::rotate(Theta/contElem);
       ArUtil::sleep(300);
    }

}


void Robot::PrintMap(){
    cout<<"------------------------------------------------------------------"<<endl;
    for (int l = 0; l < 51; l++) {
        for (int c = 0; c < 51; c++) {
          //  auxl+= (" " + this->MapR[l][c]);
            if(this->MapR[l][c]==0){
            cout<<" ";
            }else if(this->MapR[l][c]==13){
                cout<<" ";
            }else{
                cout<<this->MapR[l][c];
            }
        }
        cout<<"\n";
    }

}

bool Robot::maketri(){
    for (int var = 0; var < 4; var++) {
        ArRobot::move(1000);
        ArUtil::sleep(6000);
        Robot::rotate(90);
        ArUtil::sleep(1000);
    }
    return true;
}


int Robot::wardOff(){
    ArUtil::sleep(1000);

    int sec_dist = 4900;
    //int sleeptime = 1000;
    int sonardist[8];
    for(int i = 0;i<8;i++)
    {
        sonardist[i]= ArRobot::getSonarRange(i);
    }

    // cout<<"Estou em:"<<<<" , " <<ArRobot::getY()<<endl;


    int meXGlobal = 15 + (ArRobot::getX() /100);
    int meYGlobal = 15 + (ArRobot::getY() /100);
    this->MapR[meXGlobal][meYGlobal]=13;
    //MapR[XGlobal][YGlobal]=1;






    for (int i = 0; i < 8; i++) {
        if( sonardist[i] < 5000){
            int angulo =0;
            if(i==0){angulo = -90;}
            if(i==1){angulo = -50;}
            if(i==2){angulo = -30;}
            if(i==3){angulo = -10;}
            if(i==4){angulo = 10;}
            if(i==5){angulo = 30;}
            if(i==6){angulo = 50;}
            if(i==7){angulo = 90;}

            // H² = A² + B²
            double Hp =  sonardist[i]; //H
            double Co = sin(angulo)*Hp; // A - representa a variacao em Y
            double Ca = sqrt((Hp*Hp) -(Co*Co)); //B - representa a variacao em X
            //cout<<"Hip: "<<Hp<<" Co: "<<Co<<" Ca: "<<Ca<<endl; ★
            int Xn = ArRobot::getSonarReading(i)->getLocalX() + Ca;
            int Yn = ArRobot::getSonarReading(i)->getLocalY() + Co;


            int XGlobal = 31 + ArRobot::getX() + (Xn)/100;
            int YGlobal = 31 + ArRobot::getY() + +(Yn)/100;
            //MapR[XGlobal][YGlobal]=1;

            this->MapR[XGlobal][YGlobal]=i;
            //cout<<"Sonnar("<<i<<"):"<<"Vejo algo em:"<<XGlobal <<" , " <<YGlobal<<endl;
            //cout<<"Xn: "<<Xn<<" Yn:"<<Yn<<endl;
        }
    }





    double angulo = 0;
    bool obstaculo = false;



    Robot::rotate(angulo);
    //cout<<" Rotacionando : "<<angulo;
    ArUtil::sleep(600);

    return 0;
    }


bool Robot::shutdown()
{
    this->thread->exit();
    this->stopRunning();
    this->waitForRunExit();
    this->sick.stopRunning();
    this->sick.disconnect();
    ArRobot::disconnect();
    Aria::shutdown();
    return true;
}

void Robot::readingSensors()
{
    if(this->isConnected())
    {
        if(sick.isConnected())
        {
            sick.lockDevice();
            lasers = sick.getRawReadingsAsVector();
            sick.unlockDevice();
        }
        for(int i = 0;i<8;i++)
        {
            sonars[i] = ArRobot::getSonarRange(i);
        }
    }
}

int Robot::getLaserRange(int angle)
{
    if((this->lasers == NULL) || (this->lasers->size() <= angle))
    {
        return 0;
    }

    return this->lasers->at(angle).getRange();
}

int Robot::getSonarRange(int id_sonar)
{
    if(8 <= id_sonar)
    {
        return 0;
    }

    return this->sonars[id_sonar];
}

double Robot::getNorth()
{
    return this->getTh();
}

void Robot::move(int distanceMM)
{
    ArRobot::move(distanceMM);
    ArLog::log(ArLog::Normal,"Movendo: %d",distanceMM);
    //this->move(distanceMM);
    //this->lock();
    //this->setVel2(100,100);
    //this->move(distanceMM));
    //this->unlock();
}

void Robot::rotate(int degrees)
{
    //ArLog::log(ArLog::Normal,"Rotacionando: %d",degrees);
    //this->lock();
    this->setDeltaHeading(degrees);
    //this->unlock();
}
