#ifndef SCREENSHOTREADER_H
#define SCREENSHOTREADER_H

#include "screenshotio.h"

#include <QObject>
#include <QFileInfoList>

class ScreenshotReader : public ScreenshotInput
{
    Q_OBJECT

public:
    explicit ScreenshotReader(const QString &indir,
                              const QString &pattern,
                              QObject *parent = 0);

protected:
    void run() Q_DECL_OVERRIDE;

signals:

public slots:

private:
    QFileInfoList screenshotFileinfos;
};

#endif // SCREENSHOTREADER_H
