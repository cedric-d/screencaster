#include "screenshotsender.h"

#include <QtGlobal>
#include <QtEndian>
#include <QBuffer>
#include <QStringList>

ScreenshotSender::ScreenshotSender(const QString &destination,
                                   const QString &format,
                                   const QString &quality,
                                   QObject *parent) :
    ScreenshotOutput(parent), socket(this), imgFormat(format)
{
    this->imgQuality = quality.toInt();

    const QStringList &dest = destination.split(':');
    this->destAddress = dest[0];
    this->destPort = dest[1].toUShort();

    connect(&this->socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(&this->socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
}

void ScreenshotSender::start()
{
    this->socket.connectToHost(this->destAddress, this->destPort, QIODevice::WriteOnly);
}

void ScreenshotSender::stop()
{
    this->socket.close();
}

void ScreenshotSender::connected()
{
    qDebug() << tr("Connection established with %1").arg(this->socket.peerName());

    emit ready();
}

void ScreenshotSender::error()
{
    qDebug() << tr("Failed to connect to %1: %2").arg(this->socket.peerName(), this->socket.errorString());
}

void ScreenshotSender::handleScreenshot(QImage screenshot)
{
    QBuffer img(this);
    img.open(QIODevice::WriteOnly);
    screenshot.save(&img, this->imgFormat.toLocal8Bit().constData(), this->imgQuality);
    img.close();

    const quint32 imgSize = qToBigEndian(quint32(img.buffer().size()));
    this->socket.write(reinterpret_cast<const char *>(&imgSize), sizeof(imgSize));
    this->socket.write(img.buffer());
}
