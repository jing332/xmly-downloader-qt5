#include "getaudioinforunnable.h"

#include <QDebug>

GetAudioInfoRunnable::GetAudioInfoRunnable(int trackID, int page, int pageSize)
    : audioId_(trackID), page_(page), pageSize_(pageSize) {}

void GetAudioInfoRunnable::run() {
  auto dataError = CgoGetAudioInfo(audioId_, page_, pageSize_);
  if (dataError->error) {
    qWarning() << "get audio info fail: " << dataError->error;
    emit Error(QString(dataError->error), audioId_, page_, pageSize_);
  } else {
    QList<AudioItem *> list;
    auto array = static_cast<CArray *>(dataError->data);
    for (int i = 0; i < array->length; i++) {
      auto cgoAi =
          static_cast<CgoAudioItem *>(static_cast<void **>(array->pointer)[i]);

      AudioItem *ai = new AudioItem();
      ai->id = cgoAi->id;
      ai->url = cgoAi->url;
      ai->title = cgoAi->title;
      ai->number = cgoAi->number;
      list.append(ai);
      delete cgoAi;
    }
    emit Finished(list);
  }
  delete dataError;
}
