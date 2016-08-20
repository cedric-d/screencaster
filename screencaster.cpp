#include "screencaster.h"
#include "screenshotmaker.h"
#include "screenshotreader.h"
#include "screenshotreceiver.h"
#include "screenshotwriter.h"

#include <QCommandLineParser>
#include <QApplication>
#include <QStringList>

#include <QtDebug>

static const QString APPLICATION_NAME = "screencaster";
static const QString APPLICATION_VERSION = "1.0";

static const QString DEFAULT_PORT = "1234";
static const QString DEFAULT_BIND_ADDRESS = "0.0.0.0";

static const QString DEFAULT_PATH = ".";

static const QString DEFAULT_PERIOD = "500";

static const QString DEFAULT_FILE_FORMAT = "png";
static const QString DEFAULT_FILE_QUALITY = "-1";


Screencaster::Screencaster(int & argc, char *argv[]) :
    QApplication(argc, argv), output(0), input(0)
{
    this->setApplicationName(APPLICATION_NAME);
    this->setApplicationVersion(APPLICATION_VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription(tr("Periodically take screenshots"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption listenPort(QStringList() << "p" << "port",
                                  tr("Listen connection on PORT"),
                                  "PORT", DEFAULT_PORT);
    QCommandLineOption listenAddr(QStringList() << "b" << "bind",
                                  tr("Listen connection on ADDRESS"),
                                  "ADDRESS", DEFAULT_BIND_ADDRESS);
    parser.addOption(listenPort);
    parser.addOption(listenAddr);

    QCommandLineOption period(QStringList() << "r" << "period",
                              tr("Take screenshot every PERIOD msecs"),
                              "PERIOD", DEFAULT_PERIOD);
    parser.addOption(period);

    QCommandLineOption fileFormat("format", tr("Write files in FORMAT"),
                                  "FORMAT", DEFAULT_FILE_FORMAT);
    QCommandLineOption fileQuality("quality", tr("Write files with QUALITY"),
                                   "QUALITY", DEFAULT_FILE_QUALITY);
    parser.addOption(fileFormat);
    parser.addOption(fileQuality);

    parser.addPositionalArgument("mode", tr("Operating mode"), "send|receive|write|read");

    parser.parse(this->arguments());

    const QString & mode = parser.positionalArguments().value(0);
    if (mode == "receive") {
    } else if (mode == "read") {
        parser.clearPositionalArguments();
        parser.addPositionalArgument("read", tr("Read mode"));
        parser.addPositionalArgument("DIR", tr("Read screenshot files in DIR"));
        parser.addPositionalArgument("PATTERN", tr("Read screenshot files matching PATTERN"));

        parser.process(*this);

        const QString indir = parser.positionalArguments().value(1);
        const QString pattern = parser.positionalArguments().value(2);
        if (pattern.isEmpty()) {
            qDebug() << tr("No file pattern specified");
            parser.showHelp(1);
        }

        this->input = new ScreenshotReader(indir, pattern, this);
    } else if (mode == "send") {
    } else if (mode == "write") {
        parser.clearPositionalArguments();
        parser.addPositionalArgument("write", tr("Write mode"));
        parser.addPositionalArgument("OUTDIR", tr("Write screenshots in OUTDIR"));

        parser.process(*this);

        const QString outdir = parser.positionalArguments().value(1);
        if (outdir.isEmpty()) {
            qDebug() << tr("No output directory specified");
            parser.showHelp(1);
        }

        this->output = new ScreenshotWriter(outdir,
                                            parser.value(fileFormat),
                                            parser.value(fileQuality),
                                            this);
    } else {
        qDebug() << tr("Bad arguments");
        parser.showHelp(1);;
    }

    if (this->output) {
        this->input = new ScreenshotMaker(parser.value(period).toInt(), this);

    } else {
        this->output = new ScreenshotViewer(this);
    }

    connect(this->input, SIGNAL(screenshotAvailable(QImage)), this->output, SLOT(handleScreenshot(QImage)));
    //connect(this->input, SIGNAL(finished()), this, SLOT(quit()));

    this->input->start();
    this->output->start();
}

Screencaster::~Screencaster()
{
    delete this->output;
    delete this->input;
}
