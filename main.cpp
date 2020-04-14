#include <QApplication>
#include <QDateTime>
#include <QDebug>

#include "cgo.h"
#include "ui/downloadqueuedialog.h"
#include "ui/mainwindow.h"

//自定义消息处理函数
void myMessageOutput(QtMsgType type, const QMessageLogContext &context,
                     const QString &msg) {
  QDateTime time = QDateTime::currentDateTime();
  QString strTime = time.toString("yyyy-MM-dd hh:mm:ss");
  QString strMessage = QString("\033[31m[%1] %2:%3")
                           .arg(strTime)
                           .arg(QString(context.file).remove("/gomc-qt5"))
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
  outputStream << strMessage << endl;
  outputStream.flush();
}

MainWindow *mainWin;
extern "C" void addAudioItem(void *p) {
  auto audioItem = reinterpret_cast<AudioItem *>(p);
  if (!QString(audioItem->url).isEmpty())
    emit mainWin->addAudioItemSignal(audioItem);
}

extern "C" void init() { qInstallMessageHandler(myMessageOutput); }

// 开始运行 (在Go内调用)
extern "C" int start() {
  qInfo() << "Start Qt-GUI." << QThread::currentThreadId();
  int argc = 0;
  char **argv = 0;

  QApplication app(argc, argv);
  app.setFont(QFont("Microsoft YaHei", 12));

  mainWin = new MainWindow();
  mainWin->show();

  return app.exec();
}

extern "C" void drv_cgo_callback(void *_a, void *_b) {
  Cgo::getInstance()->setCgo(_a, _b);
}
