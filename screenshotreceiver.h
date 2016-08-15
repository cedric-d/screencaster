#ifndef SCREENSHOTRECEIVER_H
#define SCREENSHOTRECEIVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class ScreenshotReceiver : public QObject
{
    Q_OBJECT
public:
    explicit ScreenshotReceiver(const QString address,
                                const QString port,
                                QObject *parent = 0);

signals:

public slots:

private slots:
    void handleNewConnection();

private:
    QTcpServer tcpServer;
    QTcpSocket * curConnection;
};

#endif // SCREENSHOTRECEIVER_H
