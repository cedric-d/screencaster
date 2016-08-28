#include "screenshotwriter.h"

#include <QtGlobal>
#include <QtDebug>

const QString ScreenshotWriter::FILE_PATTERN = "screenshot%1";

ScreenshotWriter::ScreenshotWriter(const QString &outdir,
                                   const QString &format,
                                   const QString &quality,
                                   QObject *parent) :
    ScreenshotOutput(parent), fileFormat(format), curIndex(0)
{
    this->filePattern = outdir + '/' + ScreenshotWriter::FILE_PATTERN + '.' + format;
    this->fileQuality = quality.toInt();
}

void ScreenshotWriter::handleScreenshot(QImage screenshot)
{
    const QString & fileName = this->filePattern.arg(this->curIndex, 4, 16, QChar('0'));

    qDebug() << tr("Writing file %1").arg(fileName);

    screenshot.save(fileName, this->fileFormat.toLocal8Bit().constData(), this->fileQuality);

    this->curIndex++;
}
