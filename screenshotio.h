#ifndef SCREENSHOTIO_H
#define SCREENSHOTIO_H

#include <QtGlobal>
#include <QObject>
#include <QImage>

class ScreenshotOutput : public QObject
{
    Q_OBJECT

public:
    explicit ScreenshotOutput(QObject *parent = Q_NULLPTR) :
        QObject(parent) {}
    virtual ~ScreenshotOutput() { this->stop(); }
    virtual void start() { emit ready(); }
    virtual void stop() {}

protected:

signals:
    void ready();

public slots:
    virtual void handleScreenshot(QImage screenshot) = 0;
};


class ScreenshotInput : public QObject
{
    Q_OBJECT

public:
    explicit ScreenshotInput(QObject *parent =Q_NULLPTR) :
        QObject(parent) {}
    virtual ~ScreenshotInput() { this->stop(); }
    virtual void start() { emit ready(); }
    virtual void stop() {}

protected:

signals:
    void ready();
    void screenshotAvailable(QImage screenshot);

public slots:
};

#endif // SCREENSHOTIO_H
