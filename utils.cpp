#include "utils.h"

#include <QtGlobal>
#include <QtDebug>
#include <QObject>

#include <webp/encode.h>
#include <webp/mux.h>


static int webpWriter(const uint8_t* data, size_t data_size, const WebPPicture* picture)
{
    QIODevice *device = static_cast<QIODevice *>(picture->custom_ptr);

    return (device->write(reinterpret_cast<const char *>(data), data_size) == data_size);
}


bool writeImageWebp(const QImage &screenshot, int quality, QIODevice *output)
{
    bool result = false;

    WebPConfig outConfig;
    if (!WebPConfigInit(&outConfig)) {
        qCritical() << QObject::tr("WebPConfigInit failed");
        return false;
    }
    outConfig.lossless = 1;
    outConfig.method = 6; // quality/speed trade-off (0=fast, 6=slower-better)
    if (quality != -1)
        outConfig.quality = quality;

    WebPPicture outPicture;
    if (!WebPPictureInit(&outPicture)) {
        qCritical() << QObject::tr("WebPPictureInit failed");
        return false;
    }
    outPicture.use_argb = 1;
    outPicture.width = screenshot.width();
    outPicture.height = screenshot.height();

    if (screenshot.hasAlphaChannel()) {
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        if (screenshot.format() == QImage::Format_ARGB32) {
            if (!WebPPictureImportBGRA(&outPicture, screenshot.constBits(), screenshot.bytesPerLine())) {
                qCritical() << QObject::tr("WebPPictureImportBGRA failed");
                goto error;
            }
        } else
#endif
        {
            const QImage & img = screenshot.convertToFormat(QImage::Format_RGBA8888);
            if (!WebPPictureImportRGBA(&outPicture, img.constBits(), img.bytesPerLine())) {
                qCritical() << QObject::tr("WebPPictureImportRGBA failed");
                goto error;
            }
        }
    } else {
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
        if (screenshot.format() == QImage::Format_RGB32) {
            if (!WebPPictureImportBGRX(&outPicture, screenshot.constBits(), screenshot.bytesPerLine())) {
                qCritical() << QObject::tr("WebPPictureImportBGRX failed");
                goto error;
            }
        } else
#endif
        {
            const QImage & img = screenshot.convertToFormat(QImage::Format_RGB888);
            if (!WebPPictureImportRGB(&outPicture, img.constBits(), img.bytesPerLine())) {
                qCritical() << QObject::tr("WebPPictureImportRGB failed");
                goto error;
            }
        }
    }

    outPicture.writer = webpWriter;
    outPicture.custom_ptr = output;

    if (!WebPEncode(&outConfig, &outPicture)) {
        qCritical() << QObject::tr("WebPEncode failed with code %1").arg(outPicture.error_code);
        goto error;
    }

    result = true;

error:
    WebPPictureFree(&outPicture);

    return result;
}
