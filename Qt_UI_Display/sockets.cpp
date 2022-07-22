#include "sockets.h"

Sockets::Sockets(QObject *parent) : QObject(parent)
{

}

void Sockets::doConnect()
{
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()),this, SLOT(slotConnected()));
    connect(socket, SIGNAL(disconnected()),this, SLOT(slotDisconnected()));
//    connect(socket, SIGNAL(bytesWritten(qint64)),this, SLOT(slotSend(qint64)));
    connect(socket, SIGNAL(readyRead()),this, SLOT(slotReadyRead()));

    qDebug() << "connecting...";

    // this is not blocking call

    socket->connectToHost("10.10.141.40", 7000);

    // we need to wait...
    if(!socket->waitForConnected(5000))
    {
        qDebug() << "Error: " << socket->errorString();
    }
}

void Sockets::slotConnected()
{
    qDebug() << "connected...";
    // Hey server, tell me about you.
    socket->write("Your Connection Succeed....");

}

void Sockets::slotDisconnected()
{
    qDebug() << "disconnected...";
}

void Sockets::slotSend(qint64 num)
{
    int test = num;
    QString msg = QString(test);
//    socket->putChar(num);
    socket->write(QString(msg).toUtf8());
    qDebug() << num << "  has written...";

}

void Sockets::slotReadyRead()
{
    if (socket->isReadable())
    {
        // read the data from the socket
        QString str = socket->readAll(); // needs to find another way.. readAll() might be problematic.
        qDebug() << str;
    }
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
