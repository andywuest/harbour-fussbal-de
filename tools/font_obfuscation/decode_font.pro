QT += core network
CONFIG += c++17 console
CONFIG -= app_bundle
TEMPLATE = app
TARGET = decode_font

SOURCES += \
    decode_font_cli.cpp \
    fontdecoder.cpp

HEADERS += \
    fontdecoder.h \
    agl_data.h \
    post_names.h

LIBS += -lz