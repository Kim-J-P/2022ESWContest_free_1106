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
    //QProcess* process;
    QString strRoomNum;

    //The Total Number of Rooms
    int roomCnt = 2;

    QMessageBox::StandardButton RegYesNo;

private slots:

    void time_now();
    void dotChanges();

    // Socket
    //void slotSocketSendData();

    // Page 1
    void MoveToRegPage();  // Move To Register Page
    void MoveToRtnPage();  // Move to Return page

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

};
#endif // MAINWINDOW_H
