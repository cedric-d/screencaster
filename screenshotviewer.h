#ifndef SCREENSHOTVIEWER_H
#define SCREENSHOTVIEWER_H

#include "screenshotio.h"

#include <QtGlobal>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QTimer>

class ScreenshotViewer : public ScreenshotOutput
{
    Q_OBJECT

public:
    explicit ScreenshotViewer(const int interval, QObject *parent = Q_NULLPTR);
    void start() Q_DECL_OVERRIDE;
    void stop() Q_DECL_OVERRIDE;

signals:

public slots:
    void handleScreenshot(QImage screenshot);
    void displayNextScreenshot();

protected:

private:
    QMainWindow window;
    QLabel *label;
    QPixmap pixmap;
    QTimer timer;

    QList<QImage> queue;
};

#endif // SCREENSHOTVIEWER_H
