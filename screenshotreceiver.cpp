#include "screenshotreceiver.h"

#include <QtGlobal>
#include <QtDebug>

ScreenshotReceiver::ScreenshotReceiver(const QString address, const QString port, QObject *parent) :
    QObject(parent), tcpServer(this), curConnection(0)
{
    if (!this->tcpServer.listen(QHostAddress(address), port.toUInt()))
        throw tr("Failed to start TCP server: %1").arg(this->tcpServer.errorString());

    connect(&this->tcpServer, SIGNAL(newConnection()), this, SLOT(handleNewConnection()));
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
    }

    this->curConnection = newConnection;
}
