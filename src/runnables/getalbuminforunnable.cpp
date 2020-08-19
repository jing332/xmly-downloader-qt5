#include "getalbuminforunnable.h"

#include <QDebug>

GetAlbumInfoRunnable::GetAlbumInfoRunnable(int albumID) : albumID_(albumID) {}

void GetAlbumInfoRunnable::run() {
  auto dataErr = CgoGetAlbumInfo(albumID_);
  if (dataErr->error) {
    emit Failed(QString(dataErr->error));
    delete dataErr;
    return;
  }

  auto albumInfo = static_cast<AlbumInfo*>(dataErr->data);
  delete dataErr;
  emit Succeed(albumID_, static_cast<AlbumInfo*>(albumInfo));
}
