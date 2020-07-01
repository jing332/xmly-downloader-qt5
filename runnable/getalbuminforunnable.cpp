#include "getalbuminforunnable.h"

#include <QDebug>

GetAlbumInfoRunnable::GetAlbumInfoRunnable(int audioBookId)
    : albumID_(audioBookId) {}

void GetAlbumInfoRunnable::run() {
  auto dataErr = CgoGetAlbumInfo(albumID_);
  if (dataErr->error) {
    emit Error(QString(dataErr->error));
    delete dataErr;
    return;
  }

  auto albumInfo = static_cast<AlbumInfo*>(dataErr->data);
  delete dataErr;
  emit Finished(static_cast<AlbumInfo*>(albumInfo), albumID_);
}
