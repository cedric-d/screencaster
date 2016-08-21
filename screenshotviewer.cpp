#include "screenshotviewer.h"

#include <QPainter>
#include <QPoint>
#include <QTime>

ScreenshotViewer::ScreenshotViewer(const int interval, QObject *parent) :
    ScreenshotOutput(parent), window(0), label(new QLabel(&this->window))
{
    this->window.setCentralWidget(this->label);
    this->window.showFullScreen();

    connect(&this->timer, SIGNAL(timeout()), this, SLOT(displayNextScreenshot()));
    this->timer.start(interval);
}

void ScreenshotViewer::handleScreenshot(QImage screenshot)
{
    this->queue.append(screenshot);
}

void ScreenshotViewer::displayNextScreenshot()
{
    if (this->queue.isEmpty())
        return;

    const QImage & screenshot = this->queue.takeFirst();

    qDebug(tr("Displaying screenshot taken at %1").arg(screenshot.text("timestamp")).toLocal8Bit().constData());

    if (this->pixmap.isNull()) {
        this->pixmap = QPixmap::fromImage(screenshot);
    } else {
        QPoint target(screenshot.text("xpos").toInt(), screenshot.text("ypos").toInt());
        QPainter painter(&this->pixmap);
        painter.drawImage(target, screenshot, screenshot.rect());
    }
    this->label->setPixmap(this->pixmap);
}
