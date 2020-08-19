#ifndef GETALBUMINFORUNNABLE_H
#define GETALBUMINFORUNNABLE_H

#include <QObject>
#include <QRunnable>

#include "xmlydownloader.h"

class GetAlbumInfoRunnable : public QObject, public QRunnable {
  Q_OBJECT
 public:
  GetAlbumInfoRunnable(int audioBookId);
  void run() override;

 signals:
  void Succeed(int albumID, AlbumInfo *info);
  void Failed(const QString &err);

 private:
  int albumID_;
};

#endif  // GETALBUMINFORUNNABLE_H
