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
TARGET = harbour-xkcd

QT+= sql network

CONFIG += sailfishapp c++11

SOURCES += src/harbour-xkcd.cpp \
    src/comiccontroller.cpp \
    src/comicmodel.cpp \
    src/comic.cpp

OTHER_FILES += qml/harbour-xkcd.qml \
    qml/cover/CoverPage.qml \
    rpm/harbour-xkcd.changes.in \
    rpm/harbour-xkcd.spec \
    rpm/harbour-xkcd.yaml \
    translations/*.ts \
    harbour-xkcd.desktop \
    qml/pages/ComicPage.qml \
    qml/pages/ComicListPage.qml \
    xkcd_logo.png

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/harbour-xkcd-de.ts

HEADERS += \
    src/comiccontroller.h \
    src/comicmodel.h \
    src/comic.h

RESOURCES += \
    resources.qrc

