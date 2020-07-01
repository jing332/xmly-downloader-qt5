#ifndef GETALBUMINFORUNNABLE_H
#define GETALBUMINFORUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "cgoqt/xmlydownloader.h"

class GetAlbumInfoRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  GetAlbumInfoRunnable(int audioBookId);
  void run() override;

 signals:
  void Finished(AlbumInfo *info, int audiobookId);
  void Error(const QString &err);

 private:
  int albumID_;
};

#endif  // GETALBUMINFORUNNABLE_H
