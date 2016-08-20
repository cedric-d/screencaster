#include "screenshotreader.h"

#include <QtGlobal>
#include <QDir>
#include <QImage>

#include <QtDebug>

ScreenshotReader::ScreenshotReader(const QString &indir,
                                   const QString &pattern,
                                   QObject *parent) :
    ScreenshotInput(parent)
{
    QDir dir(indir, pattern);
    this->screenshotFileinfos = dir.entryInfoList(QDir::Files);
}

void ScreenshotReader::run()
{
    for (QFileInfoList::const_iterator it = this->screenshotFileinfos.begin();
         it != this->screenshotFileinfos.end(); ++it)
    {
        qDebug(tr("Reading file %1").arg(it->absoluteFilePath()).toLocal8Bit().constData());

        QImage img(it->absoluteFilePath());
        emit screenshotAvailable(img);
    }
}
