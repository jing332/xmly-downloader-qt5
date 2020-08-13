#ifndef GETAUDIOINFORUNNABLE_H
#define GETAUDIOINFORUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "xmlydownloader.h"
#include "type.h"

class GetAudioInfoRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  GetAudioInfoRunnable(int audioId, int page, int pageSize);
  void run() override;

 signals:
  void Finished(const QList<AudioItem *> &audioItemList);
  void Error(const QString reason, int audiobookId, int page, int pageSize);

 private:
  int audioId_;
  int page_;
  int pageSize_;
};

#endif  // GETAUDIOINFORUNNABLE_H
