#include "getaudioinforunnable.h"

#include <QDebug>

GetAudioInfoRunnable::GetAudioInfoRunnable(int albumID, int pageID)
    : albumID_(albumID), pageID_(pageID) {}

void GetAudioInfoRunnable::run() {
  auto data = CgoGetAudioInfoListByPageID(albumID_, pageID_);
  if (data->error) {
    qWarning() << data->error;
    emit Failed(albumID_, QString(data->error));
  } else {
    QList<AudioInfo *> aiList;
    auto playlist = static_cast<CgoPlaylist *>(data->data);
    auto list = static_cast<CArray *>(playlist->list);
    for (int i = 0; i < list->length; i++) {
      auto cgo =
          static_cast<CgoAudioInfo *>(static_cast<void **>(list->pointer)[i]);

      AudioInfo *ai =
          new AudioInfo(cgo->id, cgo->title, cgo->mp3URL32, cgo->mp3URL64,
                        cgo->m4aURL24, cgo->m4aURL64);
      aiList.append(ai);
      delete cgo;
    }
    emit Succeed(albumID_, playlist->maxPageID, aiList);
  }
  delete data;
}
