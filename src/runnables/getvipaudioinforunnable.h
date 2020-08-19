#ifndef GETVIPAUDIOINFORUNNABLE_H
#define GETVIPAUDIOINFORUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "audioinfo.h"
#include "downloaditemdata.h"
#include "xmlydownloader.h"

class GetVipAudioInfoRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  GetVipAudioInfoRunnable(int trackID, const QString &cookie,
                          DownloadItemData *data);

 protected:
  void run() override;

 private:
  int trackID_;
  QString cookie_;
  DownloadItemData *dlItemData_;

 signals:
  void Start(int trackID, int number);
  void Failed(int trackID, const QString &err, DownloadItemData *data);
  void Finished(AudioInfo *ai, DownloadItemData *data);
};

#endif  // GETVIPAUDIOINFORUNNABLE_H
