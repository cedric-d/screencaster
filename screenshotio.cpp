#include "screenshotio.h"

void ScreenshotOutput::start()
{
    this->moveToThread(this);
    QThread::start();
}

void ScreenshotInput::start()
{
    this->moveToThread(this);
    QThread::start();
}
