#include "screenshotwriter.h"
#include "utils.h"

#include <QtGlobal>
#include <QtDebug>
#include <QFile>

const QString ScreenshotWriter::FILE_PATTERN = "screenshot%1";

ScreenshotWriter::ScreenshotWriter(const QString &outdir,
                                   const QString &format,
                                   const QString &quality,
                                   QObject *parent) :
    ScreenshotOutput(parent), fileFormat(format), curIndex(0)
{
    QString ext;
    if (format == "libwebp")
        ext = "webp";
    else
        ext = format;

    this->filePattern = outdir + '/' + ScreenshotWriter::FILE_PATTERN + '.' + ext;
    this->fileQuality = quality.toInt();
}

void ScreenshotWriter::handleScreenshot(QImage screenshot)
{
    const QString & fileName = this->filePattern.arg(this->curIndex, 4, 16, QChar('0'));

    qDebug() << tr("Writing file %1").arg(fileName);

    if (this->fileFormat == "libwebp") {
        QFile file(fileName, this);
        if (!file.open(QIODevice::WriteOnly)) {
            qCritical() << tr("Failed to open output file: %1").arg(file.errorString());
        } else {
            writeImageWebp(screenshot, this->fileQuality, &file);
            file.close();
        }
    } else {
        screenshot.save(fileName, this->fileFormat.toLocal8Bit().constData(), this->fileQuality);
    }

    this->curIndex++;
}
