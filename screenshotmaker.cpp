#include "screenshotmaker.h"

#include <QtGlobal>
#include <QGuiApplication>
#include <QPixmap>
#include <QScreen>
#include <QTime>

ScreenshotMaker::ScreenshotMaker(const int interval, QObject *parent) :
    ScreenshotInput(parent), timer(this)
{
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(shoot()));
    this->timer.setInterval(interval);
}

void ScreenshotMaker::start()
{
    this->timer.start();
    emit ready();
}

void ScreenshotMaker::stop()
{
    this->timer.stop();
}

void ScreenshotMaker::shoot(bool forceFull)
{
    // retrieve the screen
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen)
        throw tr("No screen found");

    // create the screenshot
    QImage screenshot = screen->grabWindow(0).toImage();
    const QTime &timestamp = QTime::currentTime();

    qDebug(tr("Screenshot taken at %1").arg(QTime::currentTime().toString("HH:mm:ss.zzz")).toLocal8Bit().constData());

    // create full screenshot if asked or no previous
    Screenshot curScreenshot;
    if (forceFull || this->refScreenshot.size() != screenshot.size()) {
        curScreenshot.full = true;
        curScreenshot.image = screenshot;
    } else {
        // create empty delta screenshot
        curScreenshot.full = false;
        curScreenshot.image = QImage(screenshot.size(), QImage::Format_ARGB32);
        curScreenshot.image.fill(Qt::transparent);
        curScreenshot.crop = screenshot.rect();

        int minX = INT_MAX;
        int minY = INT_MAX;
        int maxX = 0;
        int maxY = 0;

        // copy changed pixels
        for (int y = 0; y < screenshot.height(); y++) {
            QRgb * const newLine = (QRgb *)screenshot.scanLine(y);
            QRgb * const refLine = (QRgb *)this->refScreenshot.scanLine(y);

            for (int x = 0; x < screenshot.width(); x++) {
                if (newLine[x] != refLine[x]) {
                    curScreenshot.image.setPixel(x, y, newLine[x]);

                    if (x < minX) minX = x;
                    if (x > maxX) maxX = x;
                    if (y < minY) minY = y;
                    if (y > maxY) maxY = y;
                }
            }
        }

        // crop the image
        if (minX != INT_MAX) curScreenshot.crop.setLeft(minX);
        if (minY != INT_MAX) curScreenshot.crop.setTop(minY);
        if (maxX != 0)       curScreenshot.crop.setRight(maxX);
        if (maxY != 0)       curScreenshot.crop.setBottom(maxY);
        if (curScreenshot.crop != screenshot.rect())
        {
            curScreenshot.image = curScreenshot.image.copy(curScreenshot.crop);
            curScreenshot.image.setText("xpos", QString::number(curScreenshot.crop.x()));
            curScreenshot.image.setText("ypos", QString::number(curScreenshot.crop.y()));
        }
    }

    curScreenshot.image.setText("timestamp", timestamp.toString("HH:mm:ss.zzz"));

    this->refScreenshot = screenshot;

    emit screenshotAvailable(curScreenshot.image);
}
