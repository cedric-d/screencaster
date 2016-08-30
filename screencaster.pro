QT += gui network widgets

TARGET = screencaster

QMAKE_CXXFLAGS += -march=native

LIBS += -lwebp

HEADERS += \
    screenshotmaker.h \
    screenshotsender.h \
    screenshotreceiver.h \
    screenshotwriter.h \
    screencaster.h \
    screenshotio.h \
    screenshotreader.h \
    screenshotviewer.h \
    utils.h

SOURCES += \
    screenshotmaker.cpp \
    main.cpp \
    screenshotsender.cpp \
    screenshotreceiver.cpp \
    screenshotwriter.cpp \
    screencaster.cpp \
    screenshotreader.cpp \
    screenshotviewer.cpp \
    utils.cpp
