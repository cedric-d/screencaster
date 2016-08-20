#ifndef SCREENCASTER_H
#define SCREENCASTER_H

#include "screenshotmaker.h"
#include "screenshotio.h"
#include "screenshotviewer.h"

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

private:
    ScreenshotOutput * output;
    ScreenshotInput * input;
};

#endif // SCREENCASTER_H
