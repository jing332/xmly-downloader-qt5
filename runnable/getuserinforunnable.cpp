#include "getuserinforunnable.h"

GetUserInfoRunnable::GetUserInfoRunnable(const QString &cookie, QObject *parent)
    : QObject(parent), cookie_(cookie) {}

void GetUserInfoRunnable::run() {
  auto dataErr =
      Cgo::getInstance()->cgo_getUserInfo(cookie_.toStdString().c_str());
  if (dataErr->error) {
    emit Error(QString(dataErr->error));
    delete dataErr;
    return;
  }

  emit Finisehd(static_cast<UserInfo *>(dataErr->data));
}
