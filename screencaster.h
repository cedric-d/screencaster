#ifndef SCREENCASTER_H
#define SCREENCASTER_H

#include "screenshotmaker.h"
#include "screenshotio.h"

#include <QGuiApplication>
#include <QObject>
#include <QTimer>

class Screencaster : public QGuiApplication
{
    Q_OBJECT

public:
    explicit Screencaster(int & argc, char *argv[]);
    ~Screencaster();

signals:

public slots:

private:
    QTimer timer;
    ScreenshotMaker * maker;
    ScreenshotOutput * output;
    ScreenshotInput * input;
};

#endif // SCREENCASTER_H
