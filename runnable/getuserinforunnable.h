#ifndef GETUSERINFORUNNABLE_H
#define GETUSERINFORUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "cgoqt/xmlydownloader.h"

class GetUserInfoRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  explicit GetUserInfoRunnable(const QString &cookie,
                               QObject *parent = nullptr);

 protected:
  void run() override;

 signals:
  void Error(const QString &err);
  void Finisehd(UserInfo *ui);

 private:
  QString cookie_;
};

#endif  // GETUSERINFORUNNABLE_H
