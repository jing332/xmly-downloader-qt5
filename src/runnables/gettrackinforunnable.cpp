#include "gettrackinforunnable.h"

#include <QDebug>

GetTrackInfoRunnable::GetTrackInfoRunnable(int albumID, int pageID, bool isAsc)
    : albumID_(albumID), pageID_(pageID), isAsc_(isAsc) {}

void GetTrackInfoRunnable::run() {
  auto data = CgoGetTrackList(albumID_, pageID_, int(isAsc_));
  if (data->error) {
    qWarning() << data->error;
    emit Failed(albumID_, QString(data->error));
  } else {
    QList<TrackInfo *> aiList;
    auto playlist = static_cast<CgoTrackList *>(data->data);
    auto list = static_cast<CArray *>(playlist->list);
    for (int i = 0; i < list->length; i++) {
      auto cgo =
          static_cast<CgoTrackInfo *>(static_cast<void **>(list->pointer)[i]);

      TrackInfo *ai =
          new TrackInfo(cgo->id, cgo->title, cgo->duration, cgo->mp3URL32,
                        cgo->mp3URL64, cgo->m4aURL24, cgo->m4aURL64);
      aiList.append(ai);
      delete cgo;
    }
    emit Succeed(albumID_, playlist->maxPageID, aiList);
  }
  delete data;
}
