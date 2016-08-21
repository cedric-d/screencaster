#ifndef SCREENSHOTIO_H
#define SCREENSHOTIO_H

#include <QtGlobal>
#include <QObject>
#include <QImage>

class ScreenshotOutput : public QObject
{
    Q_OBJECT

public:
    explicit ScreenshotOutput(QObject *parent = 0) :
        QObject(parent) {}

protected:

signals:

public slots:
    virtual void handleScreenshot(QImage screenshot) = 0;
};


class ScreenshotInput : public QObject
{
    Q_OBJECT

public:
    explicit ScreenshotInput(QObject *parent =0) :
        QObject(parent) {}

protected:

signals:
    void screenshotAvailable(QImage screenshot);

public slots:
};

#endif // SCREENSHOTIO_H
