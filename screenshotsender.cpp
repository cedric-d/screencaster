#include "screenshotsender.h"

#include <QtGlobal>

ScreenshotSender::ScreenshotSender(QObject *parent) :
    ScreenshotOutput(parent)
{
}

void ScreenshotSender::handleScreenshot(QImage screenshot)
{
    throw QString("TODO");
}
