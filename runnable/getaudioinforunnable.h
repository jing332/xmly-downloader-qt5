#ifndef GETAUDIOINFORUNNABLE_H
#define GETAUDIOINFORUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "cgo.h"

class GetAudioInfoRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  GetAudioInfoRunnable(int audioId, int page, int pageSize);
  void run() override;

 signals:
  void finished(const QList<AudioItem *> &audioItemList);
  void error(const QString reason, int audiobookId, int page, int pageSize);

 private:
  int audioId_;
  int page_;
  int pageSize_;
};

#endif  // GETAUDIOINFORUNNABLE_H
