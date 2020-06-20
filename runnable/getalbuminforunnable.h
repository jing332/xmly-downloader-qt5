#ifndef GETALBUMINFORUNNABLE_H
#define GETALBUMINFORUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "cgo.h"

class GetAlbumInfoRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  GetAlbumInfoRunnable(int audioBookId);
  void run() override;

 signals:
  void Finished(AlbumInfo *info, int audiobookId);

 private:
  int albumID_;
};

#endif  // GETALBUMINFORUNNABLE_H
