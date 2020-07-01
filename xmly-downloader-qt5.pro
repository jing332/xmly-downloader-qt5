QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = xmly-downloader-qt5
TEMPLATE = app
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    runnable/downloadrunnable.cpp \
    runnable/downloadvipfilerunnable.cpp \
    runnable/getalbuminforunnable.cpp \
    runnable/getaudioinforunnable.cpp \
    runnable/getuserinforunnable.cpp \
    runnable/getvipaudioinforunnable.cpp \
    ui/cookieinputdialog.cpp \
    ui/downloadqueuedialog.cpp \
    ui/downloadtaskitemwidget.cpp \
    ui/mainwindow.cpp

HEADERS += \
  appevent.h \
  main.h \
  runnable/downloadrunnable.h \
  runnable/downloadvipfilerunnable.h \
  runnable/getalbuminforunnable.h \
  runnable/getaudioinforunnable.h \
  runnable/getuserinforunnable.h \
  runnable/getvipaudioinforunnable.h \
  type.h \
  ui/cookieinputdialog.h \
  ui/downloadqueuedialog.h \
  ui/downloadtaskitemwidget.h \
  ui/mainwindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

FORMS += \
  ui/cookieinputdialog.ui \
  ui/downloadqueuedialog.ui \
  ui/mainwindow.ui

RESOURCES += \
  res/xmly-downloader-qt5.qrc \
  res/qss.qrc

LIBS += -L$$PWD/cgoqt -lxmlydownloader
