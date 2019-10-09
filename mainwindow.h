#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "worker.h"
#include <QMainWindow>
#include <QSerialPortInfo>
#include <QTime>
#include <QTimer>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    QThread* thread;
    Worker* worker;

private slots:

    void showTime();
    void showStopClock();

    void on_start_button_clicked();

    void on_stop_button_clicked();

    void on_pushButton_clicked();

    void on_btn_startWorkerThread_clicked();

    void on_btn_stopWorkThread_clicked();

    void on_btn_toggleLB_clicked();
    void startTiming();
    void stopTiming();

    void on_btn_switchMode_clicked();

private:
    Ui::MainWindow *ui;
    QTime t;
    QTimer *stopwatchtimer;
    QSerialPortInfo selectedInfo;
    bool automatedMode = true;
    bool lbswitched = false;

    void fillSerialPorts();
    QString getPortRealName(QSerialPortInfo info);
    void setAutomatedMode();
    void setManualMode();


};

#endif // MAINWINDOW_H
