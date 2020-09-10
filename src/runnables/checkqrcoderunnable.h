#ifndef CHECKQRCODERUNNABLE_H
#define CHECKQRCODERUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "xmlydownloader.h"

class CheckQRCodeRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  CheckQRCodeRunnable(const QString &qrID);
  void Stop();

 protected:
  void run() override;

 signals:
  void Succeed(const QString &Cookie);

 private:
  QString qrID_;
  bool stop_ = false;
};

#endif  // CHECKQRCODERUNNABLE_H
