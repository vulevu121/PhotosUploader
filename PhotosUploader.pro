#-------------------------------------------------
#
# Project created by QtCreator 2019-07-29T22:07:37
#
#-------------------------------------------------

QT += widgets network networkauth gui core
QT += webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PhotosUploader
TEMPLATE = app

win32:VERSION = 1.1
else:VERSION = 1.1

RC_ICONS = camera.ico

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        SettingsDialog.cpp \
        gmail.cpp \
        googleoauth2.cpp \
        googlephoto.cpp \
        googlephotoqueu.cpp \
        main.cpp \
        MainWindow.cpp

HEADERS += \
        MainWindow.h \
        SettingsDialog.h \
        gmail.h \
        googleoauth2.h \
        googlephoto.h \
        googlephotoqueu.h

FORMS += \
        MainWindow.ui \
        SettingsDialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    graphics.qrc

DISTFILES += \
    Upload Log.json
