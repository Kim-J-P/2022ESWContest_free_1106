#include "sockets.h"

Sockets::Sockets(QObject *parent) : QObject(parent)
{
    timer_delay = new QTimer(this);
    connect(timer_delay, SIGNAL(timeout()), this, SLOT(delay()));
    timer_delay->start(50);
}

void Sockets::delay()
{
}

void Sockets::processReg()
{
    QProcess processReg;
    processReg.start("/home/team5/khm/workspace/FaceRegister/build/FaceRegister");
    processReg.waitForFinished();

    processReg.exitStatus();
}

void Sockets::processRtn()
{
    QProcess processRtn;
    processRtn.start("/home/team5/khm/workspace/FaceRecognition/build/FaceRecognition");
    processRtn.waitForFinished();

    processRtn.exitStatus();
}


void Sockets::doConnect()
{
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()),this, SLOT(slotConnected()));
    connect(socket, SIGNAL(disconnected()),this, SLOT(slotDisconnected()));
    connect(socket, SIGNAL(readyRead()),this, SLOT(slotReadyRead()));

    qDebug() << "connecting...";

    socket->connectToHost(SERVERIP, SERVERPORT);

    if(!socket->waitForConnected(5000))
    {
        qDebug() << "Error: " << socket->errorString();
    }
}

void Sockets::slotConnected()
{
    qDebug() << "connected...";

    QString str = "[" + LOGID + ":" + LOGPW + "]";
    QByteArray byteStr = str.toLocal8Bit();
    socket->write(byteStr);
    socket->waitForBytesWritten();
}

void Sockets::slotDisconnected()
{
    socket->close();
    qDebug() << "disconnected...";
}

void Sockets::slotSendDoor(QString room)
{
    if (!room.isEmpty())
    {
        QByteArray msg = room.toLocal8Bit();
        socket->write(msg);
        socket->waitForBytesWritten();
        qDebug() << msg << "  has written...";
    }
}


void Sockets::slotReadyRead()
{
    if (socket->isReadable())
    {
        QString str = socket->readAll();
        qDebug() << str;
    }
}

// ===================================================

void Sockets::slotSetFile(QString filePath){
    if(filePath.isEmpty()){
        qDebug()<<"Set File Error";
        return;
    }
    QFileInfo info(filePath);
    filename = info.fileName();
    filesize = info.size();
    sendSize = 0;

    if(file.isOpen()){
        file.close();
    }
    file.setFileName(filePath);
    bool isOk = file.open(QIODevice::ReadOnly);
    if(false == isOk)
    {
        qDebug() << "Set File Name Error";
    }
    return;
}

bool Sockets::slotSendFile()
{
    qint64 len = 0;
    do
    {
        char buf[4*1024] = {0};
        len = file.read(buf,sizeof(buf));

        socket->write(buf, len);
        delay();
        //qDebug()<<socket->write(buf,len);

        sendSize += len;
        //qDebug()<<len<<sendSize;
    }while(len > 0);

     if(sendSize == filesize)
     {
         qDebug() << "Send File Success";
         file.close();
         return true;
     }
     else if (sendSize != filesize) return false;
}

// ===================================================

bool Sockets::slotRead()
{
    QString str = socket->readAll();

    // read the data from the socket
    if (str == "9")
    {
        qDebug() << "I just read " << str;
        return true;
    }
    else
    {
        qDebug() << str;
        return false;
    }
}

QString Sockets::slotReadResult()
{
    QFile file("/home/team5/khm/workspace/data/result.txt");
    if (!file.open(QFile::ReadOnly|QFile::Text))
    {
        if (!file.exists())
        {
            qDebug() << "file does not exist! needs to check...";
        }
        else
        {
            qDebug() << "Opoen File Error";
        }
    }
    QTextStream OpenResult(&file);
    QString RtnResult;
    while (!OpenResult.atEnd())
    {
        RtnResult = OpenResult.readAll();
    }
    file.close();
    return RtnResult;

}

