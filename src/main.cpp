#include <QApplication>

#include "appevent.h"
#include "ui/mainwindow.h"

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
