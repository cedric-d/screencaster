#ifndef SCREENSHOTMAKER_H
#define SCREENSHOTMAKER_H

#include "screenshotio.h"

#include <QImage>
#include <QObject>
#include <QRect>
#include <QTimer>

struct Screenshot
{
    QImage image;
    bool full;
    QRect crop;
};

class ScreenshotMaker : public ScreenshotInput
{
    Q_OBJECT

public:
    explicit ScreenshotMaker(const int interval, QObject *parent = 0);

signals:

public slots:
    void shoot(bool forceFull = false);
    void start();

private:
    QTimer timer;
    QImage refScreenshot;
};

#endif // SCREENSHOTMAKER_H
