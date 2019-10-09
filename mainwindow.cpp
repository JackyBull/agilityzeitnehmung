#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worker.h"
#include <QDebug>
#include<QTime>
#include<QTimer>
#include<QDateTime>


#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start();
    t = QTime(0,0,0,0);
    QString timer_text = t.toString("mm:ss,z");
    ui->timer->setText(timer_text);
    stopwatchtimer = new QTimer(this);
    connect(stopwatchtimer,SIGNAL(timeout()),this,SLOT(showStopClock()));
    ui->lbl_LBStatus->setText("Standard: 1=Start, 2=Ziel");
    ui->btn_toggleLB->setDisabled(true);
    this->fillSerialPorts();
    ui->statusBar->setStyleSheet("color: red; font-weight: bold");
    ui->statusBar->showMessage("Bitte Anschluss auswählen und bestätigen");
    setAutomatedMode();
}

void MainWindow::fillSerialPorts(){
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QString realName = getPortRealName(info);
        ui->comboBox->addItem(realName + " (" +info.portName() +")");
    }
}

QString MainWindow::getPortRealName(QSerialPortInfo portInfo){
    QSerialPort serial;
    QString unknown( "unbekannt");
    serial.setPort(portInfo);
    serial.setBaudRate(QSerialPort::Baud9600);
    serial.setDataBits(QSerialPort::Data8);
    serial.setParity(QSerialPort::NoParity);
    serial.setStopBits(QSerialPort::OneStop);
    serial.setFlowControl(QSerialPort::NoFlowControl);

    if (serial.open(QIODevice::ReadWrite)){
        QByteArray way;
        QString str("WAY");
        way = str.toUtf8();

        serial.write(way);
        //serial.flush();
        serial.waitForReadyRead(1000);

        static QByteArray byteArray;
        byteArray = serial.readAll();

        if(!QString(byteArray).contains("\n")){
            return unknown;
        }else{
            QString data = QString(byteArray).remove("\r").remove("\n");
            if(data.length()>0 ){
                qDebug() << data;
                if(data=="OeSAZ");
                return "ÖGV Salzburg Agility Zeitnehmung";
            }
        return unknown;
        }
    }
    return unknown;
}

void MainWindow::showTime()
{
    QTime time = QTime::currentTime();
    QString time_text = time.toString("hh:mm");
    if ((time.second() %2 ) == 0)
    {
        time_text[2] = ' ';
        time_text[5] = ' ';
    }

    ui->Digital_clock->setText(time_text);

}

void MainWindow::showStopClock()
{
    t = t.addMSecs(10);
    //qDebug() << t; //.toString("mm:ss,z")
    QString timer_text = t.toString("mm:ss,z");
    ui->timer->setText(timer_text);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_start_button_clicked()
{
   this->startTiming();
}

void MainWindow::startTiming(){
    qDebug() << "Start stop watch!";
    stopwatchtimer->start(10);
}

void MainWindow::stopTiming(){
    qDebug() << "stop stop watch!";
    stopwatchtimer->stop();
}

void MainWindow::on_stop_button_clicked()
{
    this->stopTiming();
}

void MainWindow::on_pushButton_clicked()
{
    t=QTime(0,0,0,0);
    QString timer_text = t.toString("mm:ss,z");
    ui->timer->setText(timer_text);

}

void MainWindow::on_btn_startWorkerThread_clicked()
{
    QString portName = "ttyUSB0";
    this->thread = new QThread();
    this->worker = new Worker();
    if(this->lbswitched){
        worker->toggleLbSwitched();
    }
    if(ui->comboBox->currentText()!="-- Auswählen --"){
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
            if(ui->comboBox->currentText().contains(info.portName())){
                selectedInfo = info;
            }
        }

    }

    if(selectedInfo.isNull()){
        ui->statusBar->setStyleSheet("color: red; font-weight: bold");
        ui->statusBar->showMessage("Bitte Anschluss auswählen und bestätigen");
        return;
    }else{
        ui->statusBar->setStyleSheet("color: green; font-weight: normal");
        ui->statusBar->showMessage("Bereit");
    }
    //QSerialPort* serial;
    worker->setPortInfo(selectedInfo);
    worker->moveToThread(thread);
    //connect(worker,SIGNAL(error(QString)),this,SLOT(errorString(QString)));
    connect(thread, SIGNAL (started()), worker, SLOT (process()));
    connect(thread, SIGNAL (finished()),worker, SLOT (onAbort()));
    connect(worker, SIGNAL (abort()), worker, SLOT (onAbort()));
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    connect(worker, SIGNAL(startTiming()), this, SLOT (startTiming()));
    connect(worker, SIGNAL(stopTiming()), this, SLOT (stopTiming()));
    thread->start();
    ui->btn_toggleLB->setDisabled(false);

}



void MainWindow::on_btn_stopWorkThread_clicked()
{
    qDebug("Call Stop Worker Thread");
    emit worker->onAbort();
    ui->btn_toggleLB->setDisabled(true);
    ui->statusBar->setStyleSheet("color: red; font-weight: bold");
    ui->statusBar->showMessage("Bitte Anschluss auswählen und erneut bestätigen");
}

void MainWindow::on_btn_toggleLB_clicked()
{
    this->lbswitched = this->worker->toggleLbSwitched();
    if(this->lbswitched){
        ui->lbl_LBStatus->setText("Vertauscht: 1=Ziel, 2=Start");
    }else{
        ui->lbl_LBStatus->setText("Standard: 1=Start, 2=Ziel");
    }

}

void MainWindow::on_btn_switchMode_clicked()
{
    if(this->automatedMode){ // switch to manual mode
        setManualMode();

    }else{ // auf automatischen Modus wechseln
        setAutomatedMode();
    }
    automatedMode = !automatedMode;
}

void MainWindow::setManualMode(){
    ui->btn_switchMode->setText("Auf automatischen Modus wechseln");
    ui->start_button->setDisabled(false);
    ui->stop_button->setDisabled(false);
    ui->btn_startWorkerThread->setDisabled(true);
    ui->comboBox->setDisabled(true);
    ui->btn_toggleLB->setDisabled(true);
    ui->statusBar->setStyleSheet("color: black; ");
    ui->statusBar->showMessage("Manueller Modus aktiv");
    if(thread->isRunning()){
        emit worker->onAbort();
    }

}

void MainWindow::setAutomatedMode(){
    ui->btn_switchMode->setText("Auf manuellen Modus wechseln");
    ui->start_button->setDisabled(true);
    ui->stop_button->setDisabled(true);
    ui->btn_startWorkerThread->setDisabled(false);
    ui->comboBox->setDisabled(false);

    emit on_btn_startWorkerThread_clicked();

}
