#include "robot.h"
#include <list>
#include <iostream>
#include <math.h>
#include "tabuleiro.h"

using::std::cout;
using::std::endl;




Robot::Robot(int *argc, char **argv, tabuleiro *board):
    QObject(),
    ArRobot(),
    parser(argc,argv),
    board(board)
{
    robotConnector = new ArRobotConnector(&parser,this);
    laserConnector = new ArLaserConnector(&parser,this,robotConnector);

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
    //board.GenerateButtons();
    //this->board.GenerateButtons();


    //parser->addDefaultArgument("-rh 192.168.1.11 -remoteLaserTcpPort 10002");
    parser.addDefaultArgument("-rh 127.0.0.1:8101");

    this->addRangeDevice(&sick);
    if(!robotConnector->connectRobot())
    {
        ArLog::log(ArLog::Terse,"Ops... falha ao conectar ao servidor do robo.");
        this->shutdown();
        return false;
    }

    cout << "AQUI!!!" << endl;

    ArLog::log(ArLog::Normal,"Robot connected");
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

    // Vai dar merda
//    connect(this,SIGNAL(updateDesenho(int[],double,double,double)),this->board,SLOT(updateDesenho(int[],double,double,double)));
    connect(this,SIGNAL(updateDesenho(Robot*)),this->board,SLOT(updateDesenho(Robot*)));

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

void Robot::WorldMap(int sonarFiles[]){


    int n = (500);
    int theta = this->getNorth(); //ArRobot::getOdometerDegrees();
    int xSonar[8];
    int ySonar[8];



    cout << "Th: " << theta << endl;

    int meXGlobal = 15 + (ArRobot::getX() /n); //Coordenadas de tabuleiro!
    int meYGlobal = 15 + (ArRobot::getY() /n);

    int vaidarmerda = (meYGlobal+100)*1000+(meXGlobal+100);

    for (int i = 0; i < 8; i++) {
        xSonar[i]=0;
        ySonar[i]=0;
    }

    for (int i = 0; i < 8; i++) {
        ArUtil::sleep(100);


        int angulo = theta;

        // cout << "Angulo: " << angulo << endl;


        //cout << vaidarmerda << "... Deu merda ?"<< endl;

        if(meXGlobal < 31 && meYGlobal < 31){
            board->colorBlock(vaidarmerda,0);
        } else {
            cout << "OUT " << endl;
        }

        //        int pos = (meXGlobal*1000+meYGlobal+100);
        //this->board.colorBlock(pos);

        //t.colorBlock(pos);
        //        this->MapR[meXGlobal][meYGlobal]=13;

        if(i==0){angulo = 90;}
        if(i==1){angulo = 50;}
        if(i==2){angulo = 30;}
        if(i==3){angulo = 10;}
        if(i==4){angulo = -10;}
        if(i==5){angulo = -30;}
        if(i==6){angulo = -50;}
        if(i==7){angulo = -90;}
        cout << "Angulo: " << angulo <<"\t Range: " << sonarFiles[i]  << endl;
        if( sonarFiles[i] < 5000){

//            ArUtil::sleep(300);
            //cout<<"Sonar: "<<i<<" Identificou um objeto à :"<<sonarFiles[i]<<endl;
            // H² = A² + B²
            double Hp =  sonarFiles[i]; //H
            double Co = sin((M_PI*(angulo+theta))/180.0)*Hp; // A - representa a variacao em Y
            double Ca = cos((M_PI*(angulo+theta))/180.0)*Hp; //B - representa a variacao em X

            int Xn = meXGlobal + Ca/n ;
            int Yn = meYGlobal - Co/n;

            /*P.Global = P.Centro  + Odometria + Posicao do Objeto*/


            // int XGlobal = meXGlobal + ( Xn)/n;
            //int YGlobal = meYGlobal + ( Yn)/n;


            /*int XGlobal = meXGlobal ;
            int YGlobal = meYGlobal ;


            if( theta>= 0 &&  theta < 90){
                XGlobal += ( Xn)/n;
                YGlobal += ( Yn)/n;
            }else if ( theta>= 90 &&  theta< 180){
                XGlobal += ( Xn)/n;
                YGlobal -= ( Yn)/n;
            }else if ( theta >= 180 &&  theta < 270){
                XGlobal -= ( Xn)/n;
                YGlobal -= ( Yn)/n;
            }else if (theta >=270 && theta <= 360) {
                XGlobal -= ( Xn)/n;
                YGlobal += ( Yn)/n;
            }

            */

            xSonar[i] =Xn;
            ySonar[i] =Yn;



            int Blc = (Xn+100)*1000+(Yn+100);
            // cout << "x " << XGlobal << "y " << YGlobal  << " Th:" << ArRobot::getOdometerDegrees() << endl;

            if(Xn < 31 && Yn < 31){
                board->colorBlock(Blc,1);
            } else {
                cout << "OUT " << endl;
            }

            //this->MapR[XGlobal][YGlobal]=i;
        }
    }


    for(int i=0, j=1;j<8;j++,i++){


        //        if((xSonar[i]!=0 || ySonar[i]!=0) && (xSonar[j]!=0 || ySonar[j]!=0)){

        //            if(xSonar[i] > xSonar[j] && ySonar[i] > ySonar[j]){

        //                for(int nx =xSonar[j]  ; nx <xSonar[i] ; nx++ ){
        //                    for(int ny = ySonar[j]; ny <  ySonar[i]  ; ny++){
        //                        int lc = (ny+100)*1000+(nx+100);
        //                        if(nx < 31 && ny < 31){
        //                            board->colorBlock(lc,1);
        //                        } else {
        //                            cout << "OUT " << endl;
        //                        }
        //                    }
        //                }
        //            }

        //            if(xSonar[i] > xSonar[j] && ySonar[i] < ySonar[j]){
        //                for(int nx =xSonar[j]  ; nx <xSonar[i] ; nx++ ){
        //                    for(int ny = ySonar[i]; ny <  ySonar[j]  ; ny++){
        //                        int lc = (ny+100)*1000+(nx+100);
        //                        if(nx < 31 && ny < 31){
        //                            board->colorBlock(lc,1);
        //                        } else {
        //                            cout << "OUT " << endl;
        //                        }
        //                    }
        //                }
        //            }

        //            if(xSonar[i] < xSonar[j] && ySonar[i] > ySonar[j]){
        //                for(int nx =xSonar[i]  ; nx <xSonar[j] ; nx++ ){
        //                    for(int ny = ySonar[j]; ny <  ySonar[i]  ; ny++){
        //                        int lc = (ny+100)*1000+(nx+100);
        //                        if(nx < 31 && ny < 31){
        //                            board->colorBlock(lc,1);
        //                        } else {
        //                            cout << "OUT " << endl;
        //                        }
        //                    }
        //                }
        //            }

        //            if(xSonar[i] < xSonar[j] && ySonar[i] < ySonar[j]){
        //                for(int nx =xSonar[i]  ; nx <xSonar[j] ; nx++ ){
        //                    for(int ny = ySonar[i]; ny <  ySonar[j]  ; ny++){
        //                        int lc = (ny+100)*1000+(nx+100);
        //                        if(nx < 31 && ny < 31){
        //                            board->colorBlock(lc,1);
        //                        } else {
        //                            cout << "OUT " << endl;
        //                        }
        //                    }
        //                }


        //            }

        //        }

        int Blc = (xSonar[i]+100)*1000+(ySonar[i]+100);
        // cout << "x " << XGlobal << "y " << YGlobal  << " Th:" << ArRobot::getOdometerDegrees() << endl;

        if(xSonar[i] < 31 && ySonar[i] < 31){
            board->colorBlock(Blc,1);
        } else {
            cout << "OUT " << endl;
        }




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
