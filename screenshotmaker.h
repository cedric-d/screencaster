#ifndef SCREENSHOTMAKER_H
#define SCREENSHOTMAKER_H

#include <QImage>
#include <QObject>
#include <QRect>

struct Screenshot
{
    QImage image;
    bool full;
    QRect crop;
};

class ScreenshotMaker : public QObject
{
    Q_OBJECT

public:
    explicit ScreenshotMaker(QObject *parent = 0);

signals:
    void shot(QImage screenshot);

public slots:
    void shoot(bool forceFull = false);

private:
    QImage refScreenshot;
};

#endif // SCREENSHOTMAKER_H
