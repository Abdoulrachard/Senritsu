QT       += core gui multimedia sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    models/song.cpp \
    models/playlist.cpp \
    models/library.cpp \
    models/playlistmodel.cpp \
    models/librarymodel.cpp

HEADERS += \
    mainwindow.h \
    models/song.h \
    models/playlist.h \
    models/library.h \
    models/playlistmodel.h \
    models/librarymodel.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
