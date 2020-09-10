#ifndef QRCODERUNNABLE_H
#define QRCODERUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "xmlydownloader.h"

class GetQRCodeRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  GetQRCodeRunnable();

 protected:
  void run() override;

 signals:
  void Succeed(const QString &qrID, const QString &imgBase64);
  void Failed(const QString &err);
};

#endif  // QRCODERUNNABLE_H
