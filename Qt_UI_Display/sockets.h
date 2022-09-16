#ifndef SOCKETS_H
#define SOCKETS_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QFileInfo>
#include <QTimer>

class Sockets : public QObject
{
    Q_OBJECT

private:
    QTcpSocket *socket;
    QString SERVERIP = "10.10.141.233"; // Raspberry Pi Server
    int SERVERPORT = 5000;
    QString LOGID = "Qt";
    QString LOGPW = "PASSWD";

public:
    explicit Sockets(QObject *parent = 0);
    void doConnect();

signals:

public slots:
    void delay();
    void processReg();
    void processRtn();

    void slotConnected();
    void slotDisconnected();
    void slotSendDoor(QString);
    void slotReadyRead();
    void slotSetFile(QString);
    bool slotSendFile();

    bool slotRead();
    QString slotReadResult();

private:
    QFile file;
    QString filename;
    qint64 filesize;
    qint64 recvSize;
    qint64 sendSize;
    QTimer *timer_delay;

};

#endif // SOCKETS_H
