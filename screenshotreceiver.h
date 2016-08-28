#ifndef SCREENSHOTRECEIVER_H
#define SCREENSHOTRECEIVER_H

#include "screenshotio.h"

#include <QtGlobal>
#include <QByteArray>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>

class ScreenshotReceiver : public ScreenshotInput
{
    Q_OBJECT

public:
    explicit ScreenshotReceiver(const QString &bind,
                                QObject *parent = Q_NULLPTR);
    void start() Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;

signals:

public slots:

private slots:
    void handleNewConnection();
    void handleNewData();

private:
    QString bindAddr;
    quint16 bindPort;
    QTcpServer tcpServer;
    QTcpSocket * curConnection;
    QByteArray img;
    QByteArray metadata;
    int curImgSize;
    int curMetadataSize;
};

#endif // SCREENSHOTRECEIVER_H
