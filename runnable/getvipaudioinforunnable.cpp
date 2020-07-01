#include "getvipaudioinforunnable.h"

GetVipAudioInfoRunnable::GetVipAudioInfoRunnable(int trackID,
                                                 const QString &cookie,
                                                 const QString &filePath)
    : trackID_(trackID), cookie_(cookie), filePath_(filePath) {}

void GetVipAudioInfoRunnable::run() {
  emit Start(trackID_);
  auto dataErr = CgoGetVipAudioInfo(
      trackID_, const_cast<char *>(cookie_.toStdString().c_str()));

  if (dataErr->error) {
    emit Error(trackID_, QString(dataErr->error));
    delete dataErr;
    return;
  }

  auto cgoAi = static_cast<CgoAudioItem *>(dataErr->data);
  auto ai = new AudioItem();
  ai->id = cgoAi->id;
  ai->url = cgoAi->url;
  ai->title = cgoAi->title;
  ai->number = cgoAi->number;

  emit Finished(trackID_, ai);
  delete cgoAi;
  delete dataErr;
}
