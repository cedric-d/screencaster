#include "screenshotreader.h"

#include <QtGlobal>
#include <QtDebug>
#include <QDir>
#include <QImage>
#include <QTimer>

ScreenshotReader::ScreenshotReader(const QString &indir,
                                   const QString &pattern,
                                   QObject *parent) :
    ScreenshotInput(parent)
{
    QDir dir(indir, pattern);
    this->screenshotFileinfos = dir.entryInfoList(QDir::Files);
}

void ScreenshotReader::start()
{
    QTimer::singleShot(0, this, SLOT(readNextScreenshot()));
    emit ready();
}

void ScreenshotReader::readNextScreenshot()
{
    if (this->screenshotFileinfos.empty())
        return;

    const QFileInfo & curFile = this->screenshotFileinfos.takeFirst();

    qDebug() << tr("Reading file %1").arg(curFile.absoluteFilePath());

    QImage img(curFile.absoluteFilePath());
    emit screenshotAvailable(img);

    QTimer::singleShot(0, this, SLOT(readNextScreenshot()));
}
