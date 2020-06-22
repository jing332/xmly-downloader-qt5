#include "getvipaudioinforunnable.h"

#include "cgo.h"

GetVipAudioInfoRunnable::GetVipAudioInfoRunnable(int trackID,
                                                 const QString &cookie,
                                                 const QString &filePath)
    : trackID_(trackID), cookie_(cookie), filePath_(filePath) {}

void GetVipAudioInfoRunnable::run() {
  emit Start(trackID_);
  auto dataErr = Cgo::getInstance()->cgo_getVipAudioInfo(
      trackID_, cookie_.toStdString().c_str());

  if (dataErr->error) {
    emit Error(trackID_, QString(dataErr->error));
    delete dataErr;
    return;
  }

  auto cgoAi = static_cast<CgoAudioItem *>(dataErr->data);
  AudioItem *ai = new AudioItem();
  emit Finished(trackID_, ai->fromCgo(cgoAi));
  delete cgoAi;
}
