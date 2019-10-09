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
    void setPortInfo(QSerialPortInfo portInfo);
    QSerialPortInfo getPortInfo();
    bool getLbSwitched();
    bool toggleLbSwitched();

private:
    QString portName;
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
