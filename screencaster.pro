QT += gui network

TARGET = screencaster

HEADERS += \
    screenshotmaker.h \
    screenshotsender.h \
    screenshotreceiver.h \
    screenshotwriter.h \
    screencaster.h \
    screenshotio.h \
    screenshotreader.h

SOURCES += \
    screenshotmaker.cpp \
    main.cpp \
    screenshotsender.cpp \
    screenshotreceiver.cpp \
    screenshotwriter.cpp \
    screencaster.cpp \
    screenshotreader.cpp \
    screenshotio.cpp
