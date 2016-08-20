#include "screenshotviewer.h"

#include <QPainter>
#include <QPoint>
#include <QThread>

ScreenshotViewer::ScreenshotViewer(QObject *parent) :
    ScreenshotOutput(parent), window(0), label(new QLabel(&this->window))
{
    this->window.setCentralWidget(this->label);
    this->window.showFullScreen();
}

void ScreenshotViewer::handleScreenshot(QImage screenshot)
{
    if (this->pixmap.isNull()) {
        this->pixmap = QPixmap::fromImage(screenshot);
    } else {
        QPoint target(screenshot.text("xpos").toInt(), screenshot.text("ypos").toInt());
        QPainter painter(&this->pixmap);
        painter.drawImage(target, screenshot, screenshot.rect());
    }
    this->label->setPixmap(this->pixmap);

    const QTime newTimestamp = QTime::fromString(screenshot.text("timestamp"), "HH:mm:ss.zzz");
    if (newTimestamp.isValid()) {
        if (!this->lastTimestamp.isNull()) {
            QThread::msleep(this->lastTimestamp.msecsTo(newTimestamp));
        }
        this->lastTimestamp = newTimestamp;
    } else if (!this->lastTimestamp.isNull()) {
        this->lastTimestamp = QTime();
    }
}
