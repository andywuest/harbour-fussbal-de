# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = harbour-fussball-de

CONFIG += sailfishapp

QT += network

SOURCES += src/harbour-fussball-de.cpp \
    src/fontobfuscation/fontdecoder.cpp \
    src/fussballbackend.cpp

HEADERS += src/constants.h \
    src/fontobfuscation/fontdecoder.h \
    src/fontobfuscation/agl_data.h \
    src/fontobfuscation/post_names.h \
    src/fussballbackend.h

LIBS += -lz

DISTFILES += qml/harbour-fussball-de.qml \
    qml/cover/CoverPage.qml \
    qml/pages/OverviewPage.qml \
    qml/pages/SettingsPage.qml \
    qml/pages/TablePage.qml \
    qml/components/GameResultListItem.qml \
    qml/components/TableListItem.qml \
    rpm/harbour-fussball-de.changes.in \
    rpm/harbour-fussball-de.changes.run.in \
    rpm/harbour-fussball-de.spec \
    translations/*.ts \
    harbour-fussball-de.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 172x172

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-fussball-de-de.ts
