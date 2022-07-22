#ifndef SOCKETS_H
#define SOCKETS_H

#include <QObject>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QDebug>

class Sockets : public QObject
{
    Q_OBJECT

public:
    explicit Sockets(QObject *parent = 0);
    void doConnect();

signals:

public slots:
    void slotConnected();
    void slotDisconnected();
    void slotSend(qint64);
    void slotReadyRead();
    bool slotRead();

private:
    QTcpSocket *socket;

};

#endif // SOCKETS_H
