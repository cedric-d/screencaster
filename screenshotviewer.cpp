#include "screenshotviewer.h"

#include <QtGlobal>
#include <QtDebug>
#include <QtEndian>
#include <QPainter>
#include <QPoint>
#include <QTime>

ScreenshotViewer::ScreenshotViewer(const int interval, QObject *parent) :
    ScreenshotOutput(parent), window(Q_NULLPTR), label(new QLabel(&this->window))
{
    this->window.setCentralWidget(this->label);
    this->window.showFullScreen();

    connect(&this->timer, SIGNAL(timeout()), this, SLOT(displayNextScreenshot()));
    this->timer.setInterval(interval);
}

void ScreenshotViewer::start()
{
    this->timer.start();
    emit ready();
}

void ScreenshotViewer::stop()
{
    this->timer.stop();
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

    qDebug() << tr("Displaying screenshot taken at %1").arg(screenshot.text("timestamp"));

    if (this->pixmap.isNull()) {
        this->pixmap = QPixmap::fromImage(screenshot);
    } else {
        QPoint target(screenshot.text("xpos").toInt(), screenshot.text("ypos").toInt());
        QPainter painter(&this->pixmap);
        painter.drawImage(target, screenshot, screenshot.rect());
    }
    this->label->setPixmap(this->pixmap);
}
