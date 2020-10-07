#ifndef DOWNLOADCHARGETRACKRUNNABLE_H
#define DOWNLOADCHARGETRACKRUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "downloaditemdata.h"
#include "trackinfo.h"
#include "xmlydownloader.h"

class DownloadChargeTrackRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  DownloadChargeTrackRunnable(int trackID, const QString &cookie,
                              DownloadItemData *dlItemData);

 protected:
  void run() override;

 private:
  TrackInfo *GetChargeTrackInfo(int trackID);
 signals:
  void GetInfoStart(int trackID, int number);
  void GetInfoSucceed(int trackID, int number);
  void GetInfoFailed(int trackID, const QString &err, DownloadItemData *data);

  void DownloadSucceed(int trackID, int number);
  void DownloadFailed(int trackID, int number, const QString &err);

 private:
  int trackID_;
  QString cookie_;
  DownloadItemData *dlItemData_;
};

#endif  // DOWNLOADCHARGETRACKRUNNABLE_H
