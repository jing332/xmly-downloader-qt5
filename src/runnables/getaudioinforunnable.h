#ifndef GETAUDIOINFORUNNABLE_H
#define GETAUDIOINFORUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "audioinfo.h"
#include "xmlydownloader.h"

class GetAudioInfoRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  GetAudioInfoRunnable(int albumID, int pageID);

 protected:
  void run() override;

 signals:
  void Succeed(int albumID, int maxPageID, const QList<AudioInfo *> &list);
  void Failed(int albumID, const QString &err);

 private:
  int albumID_;
  int pageID_;
};

#endif  // GETAUDIOINFORUNNABLE_H
