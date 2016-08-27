#include "screencaster.h"
#include "screenshotmaker.h"
#include "screenshotreader.h"
#include "screenshotreceiver.h"
#include "screenshotsender.h"
#include "screenshotviewer.h"
#include "screenshotwriter.h"

#include <QCommandLineParser>
#include <QApplication>
#include <QStringList>


static const QString APPLICATION_NAME = "screencaster";
static const QString APPLICATION_VERSION = "1.0";

static const QString DEFAULT_PORT = "1234";
static const QString DEFAULT_BIND_ADDRESS = "0.0.0.0";

static const QString DEFAULT_PATH = ".";

static const QString DEFAULT_PERIOD = "500";

static const QString DEFAULT_FILE_FORMAT = "png";
static const QString DEFAULT_FILE_QUALITY = "-1";


Screencaster::Screencaster(int & argc, char *argv[]) :
    QApplication(argc, argv), output(Q_NULLPTR), input(Q_NULLPTR)
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
        parser.clearPositionalArguments();
        parser.addPositionalArgument("receive", tr("Receive mode"));
        parser.addPositionalArgument("BIND_ADDR", tr("Receive screenshots on BIND_ADDR"));

        parser.process(*this);

        const QString bindAddr = parser.positionalArguments().value(1);
        if (bindAddr.isEmpty()) {
            qDebug() << tr("No bind address specified");
            parser.showHelp(1);
        }

        this->input = new ScreenshotReceiver(bindAddr, this);
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
        parser.clearPositionalArguments();
        parser.addPositionalArgument("send", tr("Send mode"));
        parser.addPositionalArgument("DEST_ADDR", tr("Send screenshots to DEST_ADDR"));

        parser.process(*this);

        const QString destAddr = parser.positionalArguments().value(1);
        if (destAddr.isEmpty()) {
            qDebug() << tr("No destination specified");
            parser.showHelp(1);
        }

        this->output = new ScreenshotSender(destAddr,
                                            parser.value(fileFormat),
                                            parser.value(fileQuality),
                                            this);
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
        this->output = new ScreenshotViewer(parser.value(period).toInt(), this);
    }

    connect(this->input, SIGNAL(ready()), this, SLOT(inputReady()));
    connect(this->output, SIGNAL(ready()), this, SLOT(outputReady()));

    qDebug() << tr("Starting output...");
    this->output->start();
}

void Screencaster::outputReady()
{
    qDebug() << tr("Output started, starting input...");

    connect(this->input, SIGNAL(screenshotAvailable(QImage)), this->output, SLOT(handleScreenshot(QImage)));
    //connect(this->input, SIGNAL(finished()), this, SLOT(quit()));

    this->input->start();
}

void Screencaster::inputReady()
{
    qDebug() << tr("Both input and output started");
}

Screencaster::~Screencaster()
{
    delete this->input;
    delete this->output;
}
