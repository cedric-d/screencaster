#ifndef SCREENCASTER_H
#define SCREENCASTER_H

#include "screenshotio.h"

#include <QApplication>
#include <QObject>

class Screencaster : public QApplication
{
    Q_OBJECT

public:
    explicit Screencaster(int & argc, char *argv[]);
    ~Screencaster();

signals:

public slots:

protected slots:
    void inputReady();
    void outputReady();

private:
    ScreenshotOutput * output;
    ScreenshotInput * input;
};

#endif // SCREENCASTER_H
