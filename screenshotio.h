#ifndef SCREENSHOTIO_H
#define SCREENSHOTIO_H

#include <QtGlobal>
#include <QObject>
#include <QImage>
#include <QThread>

class ScreenshotOutput : public QThread
{
    Q_OBJECT

public:
    explicit ScreenshotOutput(QObject *parent = 0) :
        QThread(0) {}

protected:

signals:

public slots:
    virtual void handleScreenshot(QImage screenshot) = 0;
    virtual void start();
};

class ScreenshotInput : public QThread
{
    Q_OBJECT

public:
    explicit ScreenshotInput(QObject *parent =0) :
        QThread(0) {}

protected:

signals:
    void screenshotAvailable(QImage screenshot);

public slots:
    virtual void start();
};

#endif // SCREENSHOTIO_H
