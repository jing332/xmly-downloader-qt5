#include "getvipaudioinforunnable.h"

GetVipAudioInfoRunnable::GetVipAudioInfoRunnable(int trackID,
                                                 const QString &cookie,
                                                 DownloadItemData *data)
    : trackID_(trackID), cookie_(cookie), dlItemData_(data) {}

void GetVipAudioInfoRunnable::run() {
  emit Start(trackID_, dlItemData_->number());
  auto dataErr = CgoGetVipAudioInfo(
      trackID_, const_cast<char *>(cookie_.toStdString().c_str()));

  if (dataErr->error) {
    emit Failed(trackID_, QString(dataErr->error), dlItemData_);
    delete dataErr;
    return;
  }

  auto cgo = static_cast<CgoAudioInfo *>(dataErr->data);
  auto ai = new AudioInfo(cgo->id, cgo->title, cgo->mp3URL32, cgo->mp3URL64,
                          cgo->m4aURL24, cgo->m4aURL64);
  emit Finished(ai, dlItemData_);
  delete cgo;
  delete dataErr;
}
