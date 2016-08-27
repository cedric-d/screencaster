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
    explicit ScreenshotMaker(const int interval, QObject *parent = Q_NULLPTR);
    void start() Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;

signals:

public slots:
    void shoot(bool forceFull = false);

protected:

private:
    QTimer timer;
    QImage refScreenshot;
};

#endif // SCREENSHOTMAKER_H
