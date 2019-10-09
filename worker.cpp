#include "worker.h"
#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>

Worker::Worker(QObject *parent) : QObject(parent)
{

}

Worker::Worker() { // Constructor
    // you could copy data from constructor arguments to internal variables here.

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

    QSerialPort serial;
    serial.setPort(portInfo);
    qDebug() << "Name : " << portInfo.portName();
    qDebug() << "Description : " << portInfo.description();
    qDebug() << "Manufacturer: " << portInfo.manufacturer();
    qDebug() << "Serialnumber: " << portInfo.serialNumber();
    serial.setBaudRate(QSerialPort::Baud9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    if (serial.open(QIODevice::ReadWrite)){
        qDebug("Start reading serial port");
        while(!this->thread_exit){
            serial.waitForReadyRead(100);

            static QByteArray byteArray;
            byteArray = serial.readAll();

            if(!QString(byteArray).contains("\n")){

            }else{
                QString data = QString(byteArray).remove("\r").remove("\n");
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
        }
            //QThread::sleep(1);
        qDebug("close serial connection");

        serial.close();

    }else{
        qDebug("Unable to open Serial port. Is there any other open connection to the serial port?");
    }

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


