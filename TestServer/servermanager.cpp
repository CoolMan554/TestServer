#include "servermanager.h"

ServerManager::ServerManager(int port, QObject *parent) : QTcpServer(parent)
{
    if(this->listen(QHostAddress::Any, 7001))
    {
        qDebug() << "ServerManager listen QHostAddress::Any Port:" << port;
    }
    else
        qDebug() << "ServerManager listen ERROR QHostAddress::Any Port:" << port;
}

ServerManager::~ServerManager()
{
    for(auto socket : vectorSockets)
        socket->deleteLater();
    delete tcpSocket;
}

void ServerManager::SendToClient(const QString message)
{
    QByteArray data;
    data.clear();
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_15);
    out << quint16(0) << message;
    out.device()->seek(0);
    out << quint16(data.size() - sizeof(quint16));
    tcpSocket->write(data);
    tcpSocket->flush();
}

void ServerManager::slotReadyRead()
{
    tcpSocket = (QTcpSocket*)sender();
    QDataStream in(tcpSocket);
    quint16 nextBlockSize = 0;
    in.setVersion(QDataStream::Qt_5_15);
    quint16 id = 0;
    QByteArray data;
    if(in.status() == QDataStream::Ok)
    {
        qDebug() << "ServerManager::slotReadyRead read";
        for(;;)
        {
            if(nextBlockSize == 0)
            {
                if(tcpSocket->bytesAvailable() < 2)
                    break;
                in >> nextBlockSize;
            }
            if(tcpSocket->bytesAvailable() < nextBlockSize)
                break;
            in >> id;
            in >> data;
            nextBlockSize = 0;
        }
        SendToClient(QString("id: %1 Сервер: Сообщение получено").arg(id));
    }
    else qDebug() << "ServerManager::slotReadyRead DataStream Error";
    countReadMessage++;
}

void ServerManager::incomingConnection(qintptr socketDescriptor)
{
    tcpSocket = new QTcpSocket;
    tcpSocket->setSocketDescriptor(socketDescriptor);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &ServerManager::slotReadyRead);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &ServerManager::Disconnect);

    vectorSockets.push_back(tcpSocket);

    qDebug() << "ServerManager::incomingConnection Client connected" << socketDescriptor;
}

void ServerManager::Disconnect()
{
    tcpSocket = (QTcpSocket*)sender();
    auto sock = std::remove_if(vectorSockets.begin(), vectorSockets.end(), [&](const QTcpSocket *curSocket)
    {
        return curSocket == tcpSocket;
    });
    if(sock != vectorSockets.end())
    {
        tcpSocket->deleteLater();
        vectorSockets.erase(sock);
        qDebug() << "ServerManager::Disconnect";
    }
}
