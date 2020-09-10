#include "checkqrcoderunnable.h"

#include <QThread>

CheckQRCodeRunnable::CheckQRCodeRunnable(const QString& qrID) : qrID_(qrID) {}

void CheckQRCodeRunnable::Stop() { stop_ = true; }

void CheckQRCodeRunnable::run() {
  while (!stop_) {
    char* cookie =
        CgoCheckQRCode(const_cast<char*>(qrID_.toStdString().c_str()));
    if (cookie && !stop_) {
      emit Succeed(cookie);
      return;
    }
    QThread::sleep(1);
  }
}
