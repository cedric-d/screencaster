#ifndef SCREENSHOTWRITER_H
#define SCREENSHOTWRITER_H

#include "screenshotio.h"

#include <QImage>
#include <QObject>

class ScreenshotWriter : public ScreenshotOutput
{
    Q_OBJECT

public:
    explicit ScreenshotWriter(const QString &outdir,
                              const QString &format,
                              const QString &quality,
                              QObject *parent = 0);

signals:

public slots:
    void handleScreenshot(QImage screenshot) Q_DECL_OVERRIDE;

private:
    QString filePattern;
    QString fileFormat;
    int fileQuality;
    quint32 curIndex;

    static const QString FILE_PATTERN;
};

#endif // SCREENSHOTWRITER_H
