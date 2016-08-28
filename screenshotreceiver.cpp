#include "screenshotreceiver.h"

#include <QtGlobal>
#include <QtDebug>
#include <QtEndian>
#include <QHostInfo>
#include <QHostAddress>
#include <QImage>
#include <QStringList>

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
            qCritical() << tr("Failed to resolve address %1: %2").arg(this->bindAddr, info.errorString());
        } else {
            addr = info.addresses().first();
        }
    }

    connect(&this->tcpServer, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));

    if (!this->tcpServer.listen(addr, this->bindPort))
        qCritical() << tr("Failed to start TCP server: %1").arg(this->tcpServer.errorString());
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
    if (!this->metadata.isEmpty() && this->curMetadataSize < this->metadata.size()) {
        const qint64 len = this->curConnection->read(this->metadata.data() + this->curMetadataSize,
                                                     this->metadata.size() - this->curMetadataSize);
        this->curMetadataSize += len;
    } else if (!this->img.isEmpty()) {
        const qint64 len = this->curConnection->read(this->img.data() + this->curImgSize,
                                                     this->img.size() - this->curImgSize);
        this->curImgSize += len;
    } else {
        quint32 imgSize;

        if (this->curConnection->bytesAvailable() < sizeof(imgSize))
            return;

        this->curConnection->read(reinterpret_cast<char *>(&imgSize), sizeof(imgSize));
        imgSize = qFromBigEndian(imgSize);

        this->img.resize(imgSize & 0x00ffffff);
        this->metadata.resize((imgSize >> 24) & 0xff);
        this->curImgSize = 0;
        this->curMetadataSize = 0;

        qDebug() << tr("Reading %1 bytes").arg(this->img.size() + this->metadata.size());
    }

    if (this->curImgSize == this->img.size()) {
        QImage img = QImage::fromData(this->img);
        if (!this->metadata.isEmpty()) {
            const QStringList &metadataList = QString::fromUtf8(this->metadata).split("|");
            img.setText("timestamp", metadataList.value(0));
            img.setText("xpos", metadataList.value(1));
            img.setText("ypos", metadataList.value(2));
        }
        emit screenshotAvailable(img);

        this->img.clear();
        this->metadata.clear();
    }

    if (this->curConnection->bytesAvailable() > 0)
        this->handleNewData();
}
