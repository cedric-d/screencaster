#ifndef SCREENSHOTSENDER_H
#define SCREENSHOTSENDER_H

#include "screenshotio.h"

#include <QtGlobal>
#include <QImage>
#include <QString>
#include <QTcpSocket>

class ScreenshotSender : public ScreenshotOutput
{
    Q_OBJECT

public:
    explicit ScreenshotSender(const QString &destination,
                              const QString &format,
                              const QString &quality,
                              QObject *parent = Q_NULLPTR);
    void start() Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;

signals:

public slots:
    void handleScreenshot(QImage screenshot) Q_DECL_OVERRIDE;

private slots:
    void connected();
    void error();

private:
    QString destAddress;
    quint16 destPort;
    QTcpSocket socket;
    QString imgFormat;
    int imgQuality;
};

#endif // SCREENSHOTSENDER_H
