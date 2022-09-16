#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QDate>
#include <QTime>
#include <QTimer>
#include <QDateTime>
#include <QMessageBox>
#include <QPalette>
#include <QComboBox>
#include <QDebug>
#include <QProcess>
#include <QByteArray>
#include <QThread>
#include "sockets.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Sockets *pSockets;

    QString* text;
    QString strRoomNum;    
    QMessageBox::StandardButton RegYesNo;

    int roomCnt = 2;
    int RegCount = 0;

private slots:

    void time_now();

    // Page 1
    void MoveToRegPage();
    void MoveToRtnPage();

    // Page 2
    bool RegisterRoom();
    int Room1Reg();
    int Room2Reg();
    void BackToPage1_Reg();

    // Page 3
    int ReturnRoom();
    void ReturnRoomCheck();
    void ReturnRoomFailed();
    void BackToPage1_Rtn();

    // Page 4
    void BackToPage1_CamReg();

    // Page 5
    void BackToPage1_CamRtn();

    // Page 6
    void BackToPage1_RtnFail();

};
#endif // MAINWINDOW_H
