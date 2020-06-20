#include "getalbuminforunnable.h"

GetAlbumInfoRunnable::GetAlbumInfoRunnable(int audioBookId)
    : albumID_(audioBookId) {}

void GetAlbumInfoRunnable::run() {
  auto info = Cgo::getInstance()->cgo_getAlbumInfo(albumID_);
  emit Finished(info, albumID_);
}
