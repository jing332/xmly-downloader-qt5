#ifndef GETAUDIOINFORUNNABLE_H
#define GETAUDIOINFORUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "trackinfo.h"
#include "xmlydownloader.h"

class GetTrackInfoRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  GetTrackInfoRunnable(int albumID, int pageID, bool isAsc);

 protected:
  void run() override;

 signals:
  void Succeed(int albumID, int maxPageID, const QList<TrackInfo *> &list);
  void Failed(int albumID, const QString &err);

 private:
  int albumID_;
  int pageID_;
  bool isAsc_;
};

#endif  // GETAUDIOINFORUNNABLE_H
