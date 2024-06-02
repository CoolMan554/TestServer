#include "servermanager.h"

quint32 ServerManager::countReadMessage{0};

ServerManager::ServerManager(int port, QObject *parent) : QTcpServer(parent)
{
    if(this->listen(QHostAddress::Any, 7001))
    {
        qDebug() << "ServerManager listen QHostAddress::Any Port:" << port;
        threadLog = new QThread();
        threadLog->setObjectName("ServerManager#0");
        moveToThread(threadLog);
        timerLog = new QTimer(this);
        timerLog->setSingleShot(false);
        timerLog->setInterval(periodStatusLog * 1000);
        timerLog->moveToThread(threadLog);
        connect(timerLog, &QTimer::timeout, this, &ServerManager::checkLogStatus, Qt::ConnectionType::QueuedConnection);
        connect(threadLog, &QThread::started, timerLog, static_cast<void(QTimer::*)()>(&QTimer::start));
        connect(threadLog, &QThread::finished, [this]()
        {
            qDebug() << "ServerManager::ServerManager::threadLog delete";
            threadLog->deleteLater();
        });
        threadLog->start();
    }
    else
        qDebug() << "ServerManager listen ERROR QHostAddress::Any Port:" << port;
}

ServerManager::~ServerManager()
{
    for(auto socket : vectorSockets)
        socket->deleteLater();

    if(timerLog)
        timerLog->stop();

    threadLog->quit();
    threadLog->wait();
}

void ServerManager::SendMessageToClient(const QByteArray message)
{
    if(tcpSocket->state() == QAbstractSocket::ConnectingState)//Отправляем сообщение при активном соединении
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
}

void ServerManager::slotReadyRead()
{
    tcpSocket = (QTcpSocket*)sender();
    QDataStream in(tcpSocket);
    quint16 nextBlockSize{0};///<Размер блока сообщений
    in.setVersion(QDataStream::Qt_5_15);
    QByteArray Data;
    Data.clear();
    quint32 id = 0;
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
            in >> id >> Data;
            QString responseMessage = QString("id: %1 Сервер: Сообщение получено").arg(id);
            SendMessageToClient(responseMessage.toUtf8());
            countReadMessage++;
            break;
        }
    }
    else qDebug() << "ServerManager::slotReadyRead DataStream Error";
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

void ServerManager::checkLogStatus()
{
    qDebug() << "ServerManager::checkLogStatus:: Количество принятых сообщений: " << countReadMessage;
}
