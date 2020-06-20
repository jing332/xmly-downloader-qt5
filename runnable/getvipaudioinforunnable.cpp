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
    emit Error(trackID_, dataErr->error);
    return;
  }

  emit Finished(trackID_, static_cast<AudioItem *>(dataErr->data));
}
