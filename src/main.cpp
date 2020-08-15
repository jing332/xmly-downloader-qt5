#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QMetaType>

#include "appevent.h"
#include "cgoqt/cgo.h"
#include "ui/mainwindow.h"
#include "xmlydownloader.h"

void OnUpdateFileLength(int id, long *contentLength, long *currentLength) {
  emit AppEvent::getInstance()->UpdateFileLength(id, contentLength,
                                                 currentLength);
}

int main(int argc, char *argv[]) {
  CgoRegisterCallback(OnUpdateFileLength);

  QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication a(argc, argv);
  MainWindow mw;
  mw.show();

  return a.exec();
}
