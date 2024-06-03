#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QDataStream>
#include <QThread>
#include <QTimer>

class ServerManager : public QTcpServer
{
    Q_OBJECT
public:
    ServerManager(int port, QObject* parent = nullptr);
    ~ServerManager();
private:        
    QTcpSocket *tcpSocket{};
    static quint32 countReadMessage;///<Счетчик сообщений
    QVector<QTcpSocket*> vectorSockets;        
    const int periodStatusLog{10};///<Период опроса принятых сообщений
    QTimer *timerLog{};///<Таймер для подсчета принятых сообщений
    void SendMessageToClient(const QByteArray message);
public slots:
    void slotReadyRead();
    void incomingConnection(qintptr socketDescriptor);
    void Disconnect();
    void checkLogStatus();
};

#endif // SERVERMANAGER_H
