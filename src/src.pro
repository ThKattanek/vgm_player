!win32:isEmpty(PREFIX):PREFIX=/usr/local

QT += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

CONFIG += link_pkgconfig

PKGCONFIG += zlib

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TARGET = vgm-player
TEMPLATE = app

VGM_PLAYER_VERSION = 0.1.0

# Versionsnummer ermitteln aus Git Tag Nummer
GIT_VERSION = $$system(git --git-dir \"../.git\" describe --always --tags)
isEmpty(GIT_VERSION) {
  GIT_VERSION = $$VGM_PLAYER_VERSION
}
DEFINES += VERSION_STRING=\\\"$$GIT_VERSION\\\"

message("VGM Player Version: " $$GIT_VERSION)

contains(QT_ARCH, x86_64){
    EMU64_ARCH = 64Bit
} else:contains(QT_ARCH, i[3456]86) {
    EMU64_ARCH = 32Bit
} else {
    EMU64_ARCH = Unknown
}

DEFINES += ARCHITECTURE_STRING=\\\"$$EMU64_ARCH\\\"

SOURCES += \
    about_window.cpp \
    gb_dmg_class.cpp \
    main.cpp \
    mainwindow.cpp \
    sn76489_class.cpp \
    vgmplayer.cpp \
    ym2612_class.cpp \
    audiogenerator.cpp \
    widgets/oscilloscope_widget.cpp

HEADERS += \
    about_window.h \
    gb_dmg_class.h \
    mainwindow.h \
    sn76489_class.h \
    vgmplayer.h \
    ym2612_class.h \
    audiogenerator.h \
    widgets/oscilloscope_widget.h

FORMS += \
    about_window.ui \
    mainwindow.ui \
    widgets/oscilloscope_widget.ui

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    vgm_player.qrc

# Installation

message(Installpath: $$PREFIX)
DEFINES += DATA_PATH=\\\"$$PREFIX\\\"

win32 {
    target.path = $$PREFIX
    vgm-files.path = $$PREFIX/vgm-files
} else {
    target.path = $$PREFIX/bin
    vgm-files.path = $$PREFIX/share/$$TARGET/vgm-files
}

# VGM Demo Files
vgm-files.CONFIG += nostrip
vgm-files.files += "../vgm-files/sms"
vgm-files.files += "../vgm-files/gameboy"

INSTALLS += target vgm-files
