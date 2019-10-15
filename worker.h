#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>

class Worker : public QObject
{
    Q_OBJECT
public:
    Worker(QObject* parent);
    Worker();
    ~Worker();
    void setPortName(QString portName);
    QString getPortName();
//    void setSerialPort(QSerialPort port);
//    QSerialPort getSerialPort();
    void setPortInfo(QSerialPortInfo portInfo);
    QSerialPortInfo getPortInfo();
    bool getLbSwitched();
    bool toggleLbSwitched();
    QString getPortRealName(QSerialPortInfo info);
    void init();

private:
    QString portName;
    QSerialPort* serial;
    QSerialPortInfo portInfo;
    bool lbSwitched=false;
    bool thread_exit=false;

signals:
    void finished();
    void error(QString err);
    void startTiming();
    void stopTiming();
    void abort();
public slots:
    void process();
    void onAbort();

};

#endif // WORKER_H
