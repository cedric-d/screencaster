#ifndef SCREENSHOTSENDER_H
#define SCREENSHOTSENDER_H

#include "screenshotio.h"

class ScreenshotSender : public ScreenshotOutput
{
    Q_OBJECT

public:
    explicit ScreenshotSender(QObject *parent = 0);

signals:

public slots:
    void handleScreenshot(QImage screenshot);

};

#endif // SCREENSHOTSENDER_H
