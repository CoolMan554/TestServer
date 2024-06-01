#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QDataStream>

class ServerManager : public QTcpServer
{
    Q_OBJECT
public:
    ServerManager(int port, QObject* parent = nullptr);
    ~ServerManager();
    QTcpSocket *tcpSocket;
private:
    QVector<QTcpSocket*> vectorSockets;
    QByteArray Data;
    quint32 countReadMessage{0};///<Счетчик сообщений
    void SendToClient(const QString message);
public slots:
    void slotReadyRead();
    void incomingConnection(qintptr socketDescriptor);
    void Disconnect();
};

#endif // SERVERMANAGER_H
