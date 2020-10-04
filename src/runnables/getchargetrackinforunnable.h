#ifndef GETVIPAUDIOINFORUNNABLE_H
#define GETVIPAUDIOINFORUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "audioinfo.h"
#include "xmlydownloader.h"

class GetChargeTrackInfoRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  GetChargeTrackInfoRunnable(int trackID, const QString &cookie);

 protected:
  void run() override;

 private:
  int trackID_;
  QString cookie_;

 signals:
  void Start(int trackID);
  void Failed(int trackID, const QString &err);
  void Succeed(const AudioInfo ai);
};

#endif  // GETVIPAUDIOINFORUNNABLE_H
