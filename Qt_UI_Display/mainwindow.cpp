#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    ui->pLNumberOfRooms->display(roomCnt);

    //Clock
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(time_now()));
    timer->start(1000);
    time_now();

    // Socket & DB Connection
    pSockets = new Sockets(this);

    // Page 1
    connect(ui->pPBMoveToReg, SIGNAL(clicked()), this, SLOT(MoveToRegPage()));
    connect(ui->pPBMoveToRtn, SIGNAL(clicked()), this, SLOT(MoveToRtnPage()));

    // Page 2
    connect(ui->pPBRoom1, SIGNAL(clicked()), this, SLOT(Room1Reg()));
    connect(ui->pPBRoom2, SIGNAL(clicked()), this, SLOT(Room2Reg()));
    connect(ui->pPBBack_Reg, SIGNAL(clicked()), this, SLOT(BackToPage1_Reg()));

    // Page 3
    connect(ui->pPBRtnCheck, SIGNAL(clicked()), this, SLOT(ReturnRoomCheck()));
    connect(ui->pPBBack_Rtn, SIGNAL(clicked()), this, SLOT(BackToPage1_Rtn()));

    // Page 4
    connect(ui->pPBBack_RegCam, SIGNAL(clicked()), this, SLOT(BackToPage1_CamReg()));

    // Page 5
    connect(ui->pPBBack_RtnCam, SIGNAL(clicked()), this, SLOT(BackToPage1_CamRtn()));

    // Page 6
    connect(ui->pPBBack_RtnFail, SIGNAL(clicked()), this, SLOT(BackToPage1_RtnFail()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Clock
void MainWindow::time_now()
{
    QDateTime time = QDateTime::currentDateTime();

    QString text = time.toString("yyyy-MM-dd hh:mm:ss");

    ui->pLCDClock1->display(text);
    ui->pLCDClock2->display(text);
    ui->pLCDClock3->display(text);
    ui->pLCDClock4->display(text);
    ui->pLCDClock5->display(text);
    ui->pLCDClock6->display(text);

}

// Page 1 ==================================================

void MainWindow::MoveToRegPage()
{
    ui->Page1Welcome->close();
    ui->Page2Register->show();
}

void MainWindow::MoveToRtnPage()
{
    ui->Page1Welcome->close();
    ui->Page3Return->show();
}

// Page 2 ==================================================

bool MainWindow::RegisterRoom()
{
    QMessageBox::StandardButton RegYesNo;
    RegYesNo = QMessageBox::question(this, "Registration Process", "Are you sure you want to register?", QMessageBox::Yes | QMessageBox::No);

    if (RegYesNo == QMessageBox::Yes)
    {
        return true;
    }
    else if (RegYesNo == QMessageBox::No)
    {
        return false;
    }
    return 0;
}

int MainWindow::Room1Reg()
{

    if (RegisterRoom() == true)
    {
        if (ui->pPBRoom1->styleSheet() == "background-color: rgb(157, 5, 104)")
        {
            QMessageBox::warning(this, "Registration Error", "This room has already registered. Please choose the other room.", QMessageBox::Ok);
            return -1;
        }
        else
        {
            ui->Page2Register->close();
            ui->Page4CamReg->show();

            // Socket Connect here
            pSockets->doConnect();
            pSockets->slotConnected();
            pSockets->processReg();

            pSockets->slotSetFile("/home/team5/khm/workspace/FaceRegister/build/data/Register.json");
            if (!pSockets->slotSendFile()) RegCount = -1;
            else RegCount = 0;

            pSockets->slotSendDoor("Room1\n");
            pSockets->slotDisconnected();

            // After Face ID Registration
            if (RegCount == -1)
            {
                QMessageBox::warning(this, "WARNING : Registration Failure", "Cannot find Face ID. Face ID Registration Failed.", QMessageBox::Ok);
                ui->Page4CamReg->close();
                ui->Page2Register->show();
            }
            else if (RegCount == 0)
            {
                ui->pPBRoom1->setStyleSheet("background-color: rgb(157, 5, 104)");
                roomCnt -= 1;

                if (roomCnt == 1) ui->pLRoomsLetter->setText("room");
                else ui->pLRoomsLetter->setText("rooms");

                ui->pLNumberOfRooms->display(roomCnt);

                return 1;
            }
        }
    }
    else return 0;
}

int MainWindow::Room2Reg()
{
    if (RegisterRoom() == true)
    {
        if (ui->pPBRoom2->styleSheet() == "background-color: rgb(157, 5, 104)")
        {
            QMessageBox::warning(this, "Registration Error", "This room has already registered. Please choose the other room.", QMessageBox::Ok);
            return -1;
        }
        else
        {
            ui->Page2Register->close();
            ui->Page4CamReg->show();

            // Socket Connection here
            pSockets->doConnect();
            pSockets->slotConnected();
            pSockets->processReg();

            pSockets->slotSetFile("/home/team5/khm/workspace/FaceRegister/build/data/Register.json");
            if (!pSockets->slotSendFile()) RegCount = -2;
            else RegCount = 0;

            pSockets->slotSendDoor("Room2\n");
            pSockets->slotDisconnected();

            // After Face ID Registration
            if (RegCount == -2)
            {
                QMessageBox::warning(this, "WARNING : Registration Failure", "Cannot find Face ID. Face ID Registration Failed.", QMessageBox::Ok);
                ui->Page4CamReg->close();
                ui->Page2Register->show();
            }
            else if (RegCount == 0)
            {
                ui->pPBRoom1->setStyleSheet("background-color: rgb(157, 5, 104)");
                roomCnt -= 1;

                if (roomCnt == 1) ui->pLRoomsLetter->setText("room");
                else ui->pLRoomsLetter->setText("rooms");

                ui->pLNumberOfRooms->display(roomCnt);

                return 2;
            }
        }
    }
    else return 0;
}

void MainWindow::BackToPage1_Reg()
{
    ui->Page2Register->close();
    ui->Page1Welcome->show();
}

// Page 3 ==================================================

int MainWindow::ReturnRoom(void)
{
    QMessageBox::StandardButton RtnYesNo;
    RtnYesNo = QMessageBox::question(this, "Return Process", "Are you sure you want to return your room?", QMessageBox::Yes|QMessageBox::No);
    if (RtnYesNo == QMessageBox::Yes)
    {
        if (ui->pCBRoomsList->currentText() == "Room 1") return 1;
        else if (ui->pCBRoomsList->currentText() == "Room 2") return 2;
    }
    else if (RtnYesNo == QMessageBox::No) return -1;

    return 0;
}

void MainWindow::ReturnRoomCheck()
{
    int temp = ReturnRoom();
    if (temp == 1)
    {
        if (ui->pPBRoom1->styleSheet() == "background-color: rgb(157, 5, 104)")
        {
            // Socket Connect here
            pSockets->doConnect();
            pSockets->slotConnected();
            pSockets->processRtn();

            pSockets->slotSetFile("/home/team5/khm/workspace/data/result.txt");
            pSockets->slotSendFile();

            pSockets->slotSendDoor("\nRoom1\n");

            pSockets->slotDisconnected();

            if (pSockets->slotReadResult() == "Correct")
            {
                qDebug() << "Face ID Authentication Success";
                ui->Page3Return->close();
                ui->Page5CamRtn->show();

                // After Face ID Authentication
                ui->pPBRoom1->setStyleSheet("background-color: rgb(0, 130, 135)");
                roomCnt += 1;

                if (roomCnt == 1) ui->pLRoomsLetter->setText("room");
                else ui->pLRoomsLetter->setText("rooms");

                ui->pLNumberOfRooms->display(roomCnt);
            }

            else if (pSockets->slotReadResult() == "Wrong")
            {
                qDebug() << "Face ID Authentication Failure";
                ui->Page3Return->close();
                ui->Page6ReturnFailed->show();
            }
        }
        else ReturnRoomFailed();
    }
    else if (temp == 2)
    {
        if (ui->pPBRoom2->styleSheet() == "background-color: rgb(157, 5, 104)")
        {
            // Socket Connection here
            pSockets->doConnect();
            pSockets->slotConnected();
            pSockets->processRtn();

            pSockets->slotSetFile("/home/team5/khm/workspace/data/result.txt");
            pSockets->slotSendFile();

            pSockets->slotSendDoor("\nRoom2\n");

            pSockets->slotDisconnected();

            if (pSockets->slotReadResult() == "Correct")
            {
                qDebug() << "Face ID Authentication Success";
                ui->Page3Return->close();
                ui->Page5CamRtn->show();

                // After Face ID Authentication
                ui->pPBRoom2->setStyleSheet("background-color: rgb(0, 130, 135)");
                roomCnt += 1;

                if (roomCnt == 1) ui->pLRoomsLetter->setText("room");
                else ui->pLRoomsLetter->setText("rooms");

                ui->pLNumberOfRooms->display(roomCnt);
            }

            else if (pSockets->slotReadResult() == "Wrong")
            {
                qDebug() << "Face ID Authentication Failure";
                ui->Page3Return->close();
                ui->Page6ReturnFailed->show();
            }

        }
        else ReturnRoomFailed();
    }
}


void MainWindow::ReturnRoomFailed()
{
    QMessageBox::warning(this, "Return Error", "This room is already empty. Please choose the other room.", QMessageBox::Ok);
}

void MainWindow::BackToPage1_Rtn()
{
    ui->Page3Return->close();
    ui->Page1Welcome->show();
}

// Page 4 ==================================================

void MainWindow::BackToPage1_CamReg()
{
    ui->Page4CamReg->close();
    ui->Page1Welcome->show();
}

// Page 5 ==================================================

void MainWindow::BackToPage1_CamRtn()
{
    ui->Page5CamRtn->close();
    ui->Page1Welcome->show();
}

// Page 6 ==================================================

void MainWindow::BackToPage1_RtnFail()
{
    ui->Page6ReturnFailed->close();
    ui->Page1Welcome->show();
}
