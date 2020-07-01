#include <QApplication>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QMetaType>

#include "appevent.h"
#include "cgoqt/cgo.h"
#include "cgoqt/xmlydownloader.h"
#include "ui/mainwindow.h"

//自定义消息处理函数
void myMessageOutput(QtMsgType type, const QMessageLogContext &context,
                     const QString &msg) {
  QDateTime time = QDateTime::currentDateTime();
  QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");
  QString strMessage =
      QString("\033[31m[%1] %2:%3")
          .arg(strTime)
          .arg(QString(context.file).remove("\\xmly-downloader-qt5"))
          .arg(context.line);
  QString strMsg;
  switch (type) {
    case QtDebugMsg:
      strMsg = QString(" [Debug] ");
      break;
    case QtInfoMsg:
      strMsg = QString(" [Info] ");
      break;
    case QtWarningMsg:
      strMsg = QString(" [Warning] ");
      break;
    case QtCriticalMsg:
      strMsg = QString(" [Critical] ");
      break;
    case QtFatalMsg:
      strMsg = QString(" [Fatal] ");
      break;
    default:
      strMsg = QString(" [Err]");
      break;
  }
  strMessage = strMessage.append(strMsg).append("\033[0m").append(msg);

  //同时用系统原来的函数完成输出打印
  FILE *device = stdout;
  if (type > QtInfoMsg) {
    device = stderr;
  }

  QTextStream outputStream(device);
  outputStream << strMessage << Qt::endl;
  outputStream.flush();
}

void OnUpdateFileLength(int id, long *contentLength, long *currentLength) {
  emit AppEvent::getInstance()->UpdateFileLength(id, contentLength,
                                                 currentLength);
}

MainWindow *win;
int main(int argc, char *argv[]) {
#ifdef QT_DEBUG
  qInstallMessageHandler(myMessageOutput);
#endif

  CgoRegisterCallback(OnUpdateFileLength);

  QApplication a(argc, argv);
  win = new MainWindow();
  win->show();

  return a.exec();
}
