#include "worker.h"
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>

Worker::Worker(QObject *parent) : QObject(parent)
{
    this->serial = new QSerialPort();
}

Worker::Worker() { // Constructor
    // you could copy data from constructor arguments to internal variables here.
    this->serial = new QSerialPort();

}

Worker::~Worker() { // Destructor
    // free resources
}



void Worker::process() {
    qDebug("Hello World");
    QString lb1on="LB1-ON";
    QString lb1off="LB1-OFF";
    QString lb2on="LB2-ON";
    QString lb2off="LB2-OFF";

   // serial.open(QIODevice::ReadWrite);
    qDebug() << "Name : " << portInfo.portName();
    qDebug() << "Description : " << portInfo.description();
    qDebug() << "Manufacturer: " << portInfo.manufacturer();
    qDebug() << "Serialnumber: " << portInfo.serialNumber();

    if (serial->isOpen()){
        qDebug("Start reading serial port");
        while(!this->thread_exit){
            serial->waitForReadyRead(100);

            static QByteArray byteArray;
            QString data;
            byteArray = serial->readAll();
            //qDebug() << byteArray;
            if(QString(byteArray).contains("\n")){
                data = QString(byteArray).remove("\r").remove("\n");
            }else{
                data = QString(byteArray);
            }
            if(data.length()>0){
                qDebug() << data;
                if(this->lbSwitched){
                    if(data==lb2off){
                        emit startTiming();
                    }
                    else if(data==lb1off){
                        emit stopTiming();
                    }
                }else{
                    if(data==lb1off){
                        emit startTiming();
                    }
                    else if(data==lb2off){
                        emit stopTiming();
                    }

                }

            }else{
                // data is empty
            }
        }
            //QThread::sleep(1);
        qDebug("close serial connection");



    }else{
        qDebug("Unable to open Serial port. Is there any other open connection to the serial port?");
    }
    qDebug("close serial connection");


    emit finished();
}

void Worker::onAbort()
{
    qDebug("Stop Worker Thread");
    emit stopTiming();
    this->thread_exit=true;

}

void Worker::setPortName(QString portName){
    this->portName = portName;
}

QString Worker::getPortName(){
    return this->portName;
}

//void Worker::setSerialPort(QSerialPort port)
//{
//    this->serial = port;
//}

//QSerialPort Worker::getSerialPort()
//{
//    return NULL;
//}


void Worker::setPortInfo(QSerialPortInfo portInfo){
    this->portInfo = portInfo;
}

QSerialPortInfo Worker::getPortInfo(){
    return this->portInfo;
}

bool Worker::getLbSwitched(){
    return this->lbSwitched;
}

bool Worker::toggleLbSwitched()
{
    this->lbSwitched = !this->lbSwitched;
    return lbSwitched;
}

QString Worker::getPortRealName(QSerialPortInfo info)
{
    QString unknown( "unbekannt");
    if(!serial){
        qDebug() << "Serial not set yet";
        this->serial = new QSerialPort();
    }
    serial->setPort(info);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    serial->open(QIODevice::ReadWrite);

    if (serial->isOpen() && serial->isWritable()){
        static QByteArray byteArray;
        serial->flush();
        QByteArray way;
        QString str= "WAY";
        qDebug() << "write 'WAY' to serial";
        way = str.toUtf8();
        qDebug() << way;
        int tries = 10;
        int k=0;
        while(k<tries){
            const qint64 bytesWritten = serial->write(way);
            if(bytesWritten == -1){
                qDebug() << "failed to write the data: " << serial->errorString();
            }else if (bytesWritten != way.size()){
                qDebug() << "failed to write all data: " << serial->errorString();
            } else{
                qDebug() << "byte written " << bytesWritten << " of " << way.size();
            }

            serial->waitForBytesWritten(100);
            serial->waitForReadyRead(100);
            byteArray = serial->readAll();
            qDebug() << byteArray;
            QString data = QString(byteArray).remove("\r").remove("\n");
            if(data.length()>0 ){
                if(data=="OeSAZ"){
                    qDebug() << data;
                    //serial->close();
                    return "ÖGV Salzburg Agility Zeitnehmung";
                }
            }

            k++;
        }
        //serial->close();


    }



    return unknown;
}

void Worker::init(){
    serial->setPort(portInfo);
}
