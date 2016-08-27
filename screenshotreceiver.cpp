#include "screenshotreceiver.h"

#include <QtGlobal>
#include <QtEndian>
#include <QHostInfo>
#include <QHostAddress>
#include <QImage>

ScreenshotReceiver::ScreenshotReceiver(const QString &bind, QObject *parent) :
    ScreenshotInput(parent), tcpServer(this), curConnection(Q_NULLPTR)
{
    const QStringList &addr = bind.split(':');
    this->bindAddr = addr[0];
    this->bindPort = addr[1].toUShort();
}

void ScreenshotReceiver::start()
{
    QHostAddress addr(QHostAddress::Any);

    if (!this->bindAddr.isEmpty()) {
        const QHostInfo &info = QHostInfo::fromName(this->bindAddr);
        if (info.addresses().isEmpty()) {
            qDebug() << tr("Failed to resolve address %1: %2").arg(this->bindAddr, info.errorString());
        } else {
            addr = info.addresses().first();
        }
    }

    connect(&this->tcpServer, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));

    if (!this->tcpServer.listen(addr, this->bindPort))
        qDebug() << tr("Failed to start TCP server: %1").arg(this->tcpServer.errorString());
}

void ScreenshotReceiver::handleNewConnection()
{
    QTcpSocket *newConnection = this->tcpServer.nextPendingConnection();
    if (!newConnection)
        return;

    qDebug() << tr("Incoming connection from %1").arg(newConnection->peerAddress().toString());

    if (this->curConnection) {
        this->curConnection->close();
        delete this->curConnection;
        this->curConnection = Q_NULLPTR;
    }

    this->curConnection = newConnection;

    connect(newConnection, SIGNAL(readyRead()), this, SLOT(handleNewData()));

    emit ready();
}

void ScreenshotReceiver::stop()
{
    if (this->curConnection) {
        this->curConnection->close();
        delete this->curConnection;
        this->curConnection = Q_NULLPTR;
    }

    this->tcpServer.close();
}

void ScreenshotReceiver::handleNewData()
{
    if (!this->img.isEmpty()) {
        const qint64 len = this->curConnection->read(this->img.data() + this->curSize, this->img.size() - this->curSize);

        this->curSize += len;

        if (this->curSize == this->img.size()) {
            const QImage &img = QImage::fromData(this->img);
            emit screenshotAvailable(img);

            this->img.clear();
        }
    } else {
        quint32 imgSize;

        if (this->curConnection->bytesAvailable() < sizeof(imgSize))
            return;

        this->curConnection->read(reinterpret_cast<char *>(&imgSize), sizeof(imgSize));
        imgSize = qFromBigEndian(imgSize);

        this->img.resize(imgSize);
        this->curSize = 0;
    }

    if (this->curConnection->bytesAvailable() > 0)
        this->handleNewData();
}
