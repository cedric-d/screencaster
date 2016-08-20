#ifndef UTILS_H
#define UTILS_H

#include <QImage>
#include <QIODevice>

extern bool writeImageWebp(const QImage &screenshot, int quality, QIODevice *output);

#endif // UTILS_H
