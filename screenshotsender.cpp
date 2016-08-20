#include "screenshotsender.h"
#include "utils.h"

#include <QtGlobal>
#include <QtDebug>
#include <QtEndian>
#include <QBuffer>
#include <QImageWriter>
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
    qCritical() << tr("Failed to connect to %1: %2").arg(this->socket.peerName(), this->socket.errorString());
}

void ScreenshotSender::handleScreenshot(QImage screenshot)
{
    QImageWriter writer;
    QBuffer img(this);
    img.open(QIODevice::WriteOnly);

    if (this->imgFormat == "libwebp") {
        if (!writeImageWebp(screenshot, this->imgQuality, &img)) {
            qCritical() << tr("Failed to create image");
            return;
        }
    } else {
        writer.setDevice(&img);
        writer.setFormat(this->imgFormat.toUtf8());
        writer.setQuality(this->imgQuality);
        if (!writer.write(screenshot)) {
            qCritical() << tr("Failed to create image: %1").arg(writer.errorString());
            return;
        }
    }
    img.close();

    quint32 imgSize = img.buffer().size();
    QByteArray metadata;
    // store metadata size in upper byte if not handled by the format
    if (!writer.canWrite() || !writer.supportsOption(QImageIOHandler::Description)) {
        QStringList metadataList;
        metadataList.append(screenshot.text("timestamp"));
        metadataList.append(screenshot.text("xpos"));
        metadataList.append(screenshot.text("ypos"));
        const QString &metadataString = metadataList.join("|");
        metadata = metadataString.toUtf8();
        imgSize |= quint32(metadata.size()) << 24;
    }
    imgSize = qToBigEndian(imgSize);
    this->socket.write(reinterpret_cast<const char *>(&imgSize), sizeof(imgSize));
    this->socket.write(metadata);
    this->socket.write(img.buffer());

    qDebug() << tr("%1 bytes sent").arg(img.buffer().size() + metadata.size());
}
