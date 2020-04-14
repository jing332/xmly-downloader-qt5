#ifndef GETAUDIOBOOKINFORUNNABLE_H
#define GETAUDIOBOOKINFORUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "cgo.h"

class GetAudiobookInfoRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  GetAudiobookInfoRunnable(int audioBookId);
  void run() override;

 signals:
  void finished(AudioBookInfo *info, int audiobookId);

 private:
  int audioBookId_;
};

#endif  // GETAUDIOBOOKINFORUNNABLE_H
