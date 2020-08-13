#ifndef GETVIPAUDIOINFORUNNABLE_H
#define GETVIPAUDIOINFORUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "xmlydownloader.h"
#include "type.h"

class GetVipAudioInfoRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  GetVipAudioInfoRunnable(int trackID, const QString &cookie,
                          const QString &filePath);

 protected:
  void run() override;

 private:
  int trackID_;
  QString cookie_;
  QString filePath_;

 signals:
  void Start(int trackID);
  void Error(int trackID, const QString &err);
  void Finished(int trackID, AudioItem *ai);
};

#endif  // GETVIPAUDIOINFORUNNABLE_H
