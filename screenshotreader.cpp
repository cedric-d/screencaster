#include "screenshotreader.h"

#include <QtGlobal>
#include <QDir>
#include <QImage>
#include <QTimer>

#include <QtDebug>

ScreenshotReader::ScreenshotReader(const QString &indir,
                                   const QString &pattern,
                                   QObject *parent) :
    ScreenshotInput(parent)
{
    QDir dir(indir, pattern);
    this->screenshotFileinfos = dir.entryInfoList(QDir::Files);

    QTimer::singleShot(0, this, SLOT(readNextScreenshot()));
}

void ScreenshotReader::readNextScreenshot()
{
    if (this->screenshotFileinfos.empty())
        return;

    const QFileInfo & curFile = this->screenshotFileinfos.takeFirst();

    qDebug(tr("Reading file %1").arg(curFile.absoluteFilePath()).toLocal8Bit().constData());

    QImage img(curFile.absoluteFilePath());
    emit screenshotAvailable(img);

    QTimer::singleShot(0, this, SLOT(readNextScreenshot()));
}
