#include "screenshotreader.h"

#include <QtGlobal>
#include <QDir>

#include <QtDebug>

ScreenshotReader::ScreenshotReader(const QString &indir,
                                   const QString &pattern,
                                   QObject *parent) :
    ScreenshotInput(parent)
{
    QDir dir(indir, pattern);
    this->screenshotFiles = dir.entryList(QDir::Files);
}

void ScreenshotReader::run()
{
    for (QStringList::const_iterator it = this->screenshotFiles.begin();
         it != this->screenshotFiles.end(); ++it)
    {
        qDebug(tr("Reading file %1").arg(*it).toLocal8Bit().constData());

        QImage img(*it);
        qDebug() << img.text();
        emit screenshotAvailable(img);
    }
}
