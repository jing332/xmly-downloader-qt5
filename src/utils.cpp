#include "utils.h"

#include <QDebug>
#include <QDir>
#include <QProcess>

Utils::Utils() {}

int Utils::GetIntWidth(int n) {
  int count = 0;
  while (n != 0) {
    n = n / 10;
    count++;
  }
  return count;
}

QString Utils::GetJvmDlPath() {
  QProcess process;
  process.start("where", QStringList() << "java");
  process.waitForFinished();
  QString result = process.readAll();
  qDebug() << result;
  int index = result.indexOf("\r\n");
  QString javaPath = result.left(index);
  QDir dir(javaPath);
  dir.cd("../../");
  return dir.filePath("jvm.dll");
}
