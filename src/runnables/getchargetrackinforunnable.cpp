#include "getchargetrackinforunnable.h"

GetChargeTrackInfoRunnable::GetChargeTrackInfoRunnable(int trackID,
                                                       const QString &cookie)
    : trackID_(trackID), cookie_(cookie) {}

void GetChargeTrackInfoRunnable::run() {
  emit Start(trackID_);
  auto dataErr = CgoGetVipAudioInfo(
      trackID_, const_cast<char *>(cookie_.toStdString().c_str()));

  if (dataErr->error) {
    emit Failed(trackID_, QString(dataErr->error));
    delete dataErr;
    return;
  }

  auto cgo = static_cast<CgoTrackInfo *>(dataErr->data);
  TrackInfo ai(cgo->id, cgo->title, cgo->duration, cgo->mp3URL32, cgo->mp3URL64,
               cgo->m4aURL24, cgo->m4aURL64);
  emit Succeed(ai);
  delete cgo;
  delete dataErr;
}
