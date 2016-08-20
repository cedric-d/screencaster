#ifndef SCREENSHOTVIEWER_H
#define SCREENSHOTVIEWER_H

#include "screenshotio.h"

#include <QtGlobal>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QTime>

class ScreenshotViewer : public ScreenshotOutput
{
    Q_OBJECT
public:
    explicit ScreenshotViewer(QObject *parent = 0);

signals:

public slots:
    void handleScreenshot(QImage screenshot);

private:
    QMainWindow window;
    QLabel *label;
    QPixmap pixmap;
    QTime lastTimestamp;
};

#endif // SCREENSHOTVIEWER_H
