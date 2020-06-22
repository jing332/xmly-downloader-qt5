#include "getaudioinforunnable.h"

#include <qthread.h>

#include <QDebug>

GetAudioInfoRunnable::GetAudioInfoRunnable(int trackID, int page, int pageSize)
    : audioId_(trackID), page_(page), pageSize_(pageSize) {}

void GetAudioInfoRunnable::run() {
  auto dataError =
      Cgo::getInstance()->cgo_getAudioInfo(audioId_, page_, pageSize_);

  QString err(dataError->error);
  if (err.isEmpty()) {
    QList<AudioItem *> list;
    auto array = static_cast<Array *>(dataError->data);
    for (int i = 0; i < array->length; i++) {
      auto cgoAi = static_cast<CgoAudioItem *>(array->pointer[i]);

      AudioItem *ai = new AudioItem();
      list.append(ai->fromCgo(cgoAi));
      delete cgoAi;
    }
    emit Finished(list);
  } else {
    qWarning() << "get audio info fail: " << err;
    emit Error(err, audioId_, page_, pageSize_);
  }
}
