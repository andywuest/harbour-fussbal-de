QT += widgets network
CONFIG += c++17
TEMPLATE = app
TARGET = fussball_decoder

SOURCES += \
    main.cpp \
    fontdecoder.cpp

HEADERS += \
    fontdecoder.h \
    agl_data.h \
    post_names.h

LIBS += -lz