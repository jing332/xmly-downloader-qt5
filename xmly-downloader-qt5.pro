QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

TEMPLATE = lib

TARGET = xmly-downloader-qt5
DESTDIR = $$PWD/cgoqt

CONFIG(debug,  debug|release){
QMAKE_POST_LINK = "cd $$PWD/cgoqt && go build -v -o xmly-downloader-qt5.exe"
} else {
QMAKE_POST_LINK = "cd $$PWD/cgoqt ; go build -ldflags '-H windowsgui' -v -o xmly-downloader-qt5.exe"
}

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cgo.cpp \
    main.cpp \
    runnable/downloadrunnable.cpp \
    runnable/getaudiobookinforunnable.cpp \
    runnable/getaudioinforunnable.cpp \
    ui/downloadqueuedialog.cpp \
    ui/downloadtaskitemwidget.cpp \
    ui/mainwindow.cpp

HEADERS += \
  appevent.h \
    cgo.h \ \
  runnable/downloadrunnable.h \
  runnable/getaudiobookinforunnable.h \
  runnable/getaudioinforunnable.h \
  ui/downloadqueuedialog.h \
  ui/downloadtaskitemwidget.h \
  ui/mainwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
  ui/downloadqueuedialog.ui \
  ui/mainwindow.ui
