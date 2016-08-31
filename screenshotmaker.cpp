#include "screenshotmaker.h"

#include <QtGlobal>
#include <QtDebug>
#include <QGuiApplication>
#include <QPixmap>
#include <QScreen>
#include <QTime>

#include <xmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>

typedef void (*ImageCompare)(const QImage &refImg, const QImage &newImg, QImage &resImg);

static void compareImage_generic(const QImage &refImg, const QImage &newImg, QImage &resImg);
#ifdef __SSE2__
static void compareImage_sse2(const QImage &refImg, const QImage &newImg, QImage &resImg);
#endif
#if defined(__AVX__ ) && defined(__AVX2__)
static void compareImage_avx2(const QImage &refImg, const QImage &newImg, QImage &resImg);
#endif
#ifdef __AVX512F__
static void compareImage_avx512(const QImage &refImg, const QImage &newImg, QImage &resImg);
#endif

static ImageCompare compareImage = compareImage_generic;

static void detectCPU()
{
#if defined(__x86_64__) || defined(__i386__)
    __builtin_cpu_init();
#endif

#ifdef __AVX512F__
    if (__builtin_cpu_supports("avx512f")) {
        compareImage = compareImage_avx512;
        qDebug() << "Using AVX512 image comparison code";
    } else
#endif
#if defined(__AVX__ ) && defined(__AVX2__)
    if (__builtin_cpu_supports("avx") && __builtin_cpu_supports("avx2")) {
        compareImage = compareImage_avx2;
        qDebug() << "Using AVX2 image comparison code";
    } else
#endif
#ifdef __SSE2__
    if (__builtin_cpu_supports("sse2")) {
        compareImage = compareImage_sse2;
        qDebug() << "Using SSE2 image comparison code";
    } else
#endif
    {
        compareImage = compareImage_generic;
        qDebug() << "Using generic image comparison code";
    }
}

ScreenshotMaker::ScreenshotMaker(const int interval, QObject *parent) :
    ScreenshotInput(parent), timer(this)
{
    connect(&this->timer, SIGNAL(timeout()), this, SLOT(shoot()));
    this->timer.setInterval(interval);

    detectCPU();
}

void ScreenshotMaker::start()
{
    this->timer.start();
    emit ready();
}

void ScreenshotMaker::stop()
{
    this->timer.stop();
}

void ScreenshotMaker::shoot(bool forceFull)
{
    // retrieve the screen
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen)
        throw tr("No screen found");

    // create the screenshot
    QImage screenshot = screen->grabWindow(0).toImage();
    const QTime &timestamp = QTime::currentTime();

    qDebug() << tr("Screenshot taken at %1").arg(QTime::currentTime().toString("HH:mm:ss.zzz"));

    // create full screenshot if asked or no previous
    Screenshot curScreenshot;
    if (forceFull || this->refScreenshot.size() != screenshot.size()) {
        curScreenshot.full = true;
        curScreenshot.image = screenshot;
    } else {
        curScreenshot.full = false;
        curScreenshot.crop = screenshot.rect();

        int minX = INT_MAX;
        int minY = INT_MAX;
        int maxX = 0;
        int maxY = 0;

        // copy changed pixels
        compareImage(this->refScreenshot, screenshot, curScreenshot.image);
        /*
        for (int y = 0; y < screenshot.height(); y++) {
            QRgb * const newLine = (QRgb *)screenshot.scanLine(y);
            QRgb * const refLine = (QRgb *)this->refScreenshot.scanLine(y);

            for (int x = 0; x < screenshot.width(); x++) {
                if (newLine[x] != refLine[x]) {
                    curScreenshot.image.setPixel(x, y, newLine[x]);

                    if (x < minX) minX = x;
                    if (x > maxX) maxX = x;
                    if (y < minY) minY = y;
                    if (y > maxY) maxY = y;
                }
            }
        }

        // crop the image
        if (minX != INT_MAX) curScreenshot.crop.setLeft(minX);
        if (minY != INT_MAX) curScreenshot.crop.setTop(minY);
        if (maxX != 0)       curScreenshot.crop.setRight(maxX);
        if (maxY != 0)       curScreenshot.crop.setBottom(maxY);
        if (curScreenshot.crop != screenshot.rect())
        {
            curScreenshot.image = curScreenshot.image.copy(curScreenshot.crop);
            curScreenshot.image.setText("xpos", QString::number(curScreenshot.crop.x()));
            curScreenshot.image.setText("ypos", QString::number(curScreenshot.crop.y()));
        }
        */
    }

    curScreenshot.image.setText("timestamp", timestamp.toString("HH:mm:ss.zzz"));

    this->refScreenshot = screenshot;

    emit screenshotAvailable(curScreenshot.image);
}

void compareImage_generic(const QImage &refImg, const QImage &newImg, QImage &resImg)
{
    resImg = QImage(refImg.size(), QImage::Format_ARGB32);
    resImg.fill(Qt::transparent);

    for (int y = 0; y < newImg.height(); y++) {
        QRgb * const newLine = (QRgb *)newImg.scanLine(y);
        QRgb * const refLine = (QRgb *)refImg.scanLine(y);

        for (int x = 0; x < newImg.width(); x++) {
            if (newLine[x] != refLine[x]) {
                resImg.setPixel(x, y, newLine[x]);
            }
        }
    }
}

#ifdef __SSE2__
void compareImage_sse2(const QImage &refImg, const QImage &newImg, QImage &resImg)
{
    const size_t size = 4 * refImg.size().width() * refImg.size().height();
    uchar *data = (uchar*)_mm_malloc(size, sizeof(__m128i));

    const __m128i *refData = reinterpret_cast<const __m128i*>(refImg.constBits());
    const __m128i *newData = reinterpret_cast<const __m128i*>(newImg.constBits());
    __m128i *resData = reinterpret_cast<__m128i*>(data);

    for (int i = 0; i < (refImg.byteCount() / sizeof(__m128i)); i++) {
        const __m128i r = _mm_loadu_si128(&refData[i]);
        const __m128i n = _mm_loadu_si128(&newData[i]);
        const __m128i d = _mm_andnot_si128(_mm_cmpeq_epi32(r, n), n);
        _mm_store_si128(&resData[i], d);
    }

    resImg = QImage(data, refImg.size().width(), refImg.size().height(),
                    QImage::Format_ARGB32, _mm_free, data);
}
#endif

#if defined(__AVX__ ) && defined(__AVX2__)
void compareImage_avx2(const QImage &refImg, const QImage &newImg, QImage &resImg)
{
    const size_t size = 4 * refImg.size().width() * refImg.size().height();
    uchar *data = (uchar*)_mm_malloc(size, sizeof(__m256i));

    const __m256i *refData = reinterpret_cast<const __m256i*>(refImg.constBits());
    const __m256i *newData = reinterpret_cast<const __m256i*>(newImg.constBits());
    __m256i *resData = reinterpret_cast<__m256i*>(data);

    for (int i = 0; i < (refImg.byteCount() / sizeof(__m256i)); i++) {
        const __m256i r = _mm256_loadu_si256(&refData[i]);
        const __m256i n = _mm256_loadu_si256(&newData[i]);
        const __m256i d = _mm256_andnot_si256(_mm256_cmpeq_epi32(r, n), n);
        _mm256_store_si256(&resData[i], d);
    }

    resImg = QImage(data, refImg.size().width(), refImg.size().height(),
                    QImage::Format_ARGB32, _mm_free, data);
}
#endif

#ifdef __AVX512F__
void compareImage_avx512(const QImage &refImg, const QImage &newImg, QImage &resImg)
{
    const size_t size = 4 * refImg.size().width() * refImg.size().height();
    uchar *data = (uchar*)_mm_malloc(size, sizeof(__m512i));

    const __m512i *refData = reinterpret_cast<const __m512i*>(refImg.constBits());
    const __m512i *newData = reinterpret_cast<const __m512i*>(newImg.constBits());
    __m512i *resData = reinterpret_cast<__m512i*>(data);

    for (int i = 0; i < (refImg.byteCount() / sizeof(__m512i)); i++) {
        const __m512i r = _mm512_loadu_si512(&refData[i]);
        const __m512i n = _mm512_loadu_si512(&newData[i]);
        const __m512i d = _mm512_maskz_mov_epi32(_mm512_cmpneq_epu32_mask(r, n), n);
        _mm512_store_si512(&resData[i], d);
    }

    resImg = QImage(data, refImg.size().width(), refImg.size().height(),
                    QImage::Format_ARGB32, _mm_free, data);
}
#endif
