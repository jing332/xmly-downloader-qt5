#include "downloadchargetrackrunnable.h"

#include <QDebug>

DownloadChargeTrackRunnable::DownloadChargeTrackRunnable(
    int trackID, const QString &cookie, DownloadItemData *dlItemData)
    : trackID_(trackID), cookie_(cookie), dlItemData_(dlItemData) {}

void DownloadChargeTrackRunnable::run() {
  emit GetInfoStart(trackID_, dlItemData_->number());
  auto ai = GetChargeTrackInfo(trackID_);
  if (!ai) return;

  emit GetInfoSucceed(trackID_, dlItemData_->number());

  QString filepath = dlItemData_->saveDir() + "/" + dlItemData_->name();
  auto *err =
      CgoDownloadFile(const_cast<char *>(ai->m4aURL64().toStdString().c_str()),
                      const_cast<char *>(filepath.toStdString().c_str()),
                      dlItemData_->number());

  if (err) {
    qWarning() << QStringLiteral("download fail: {url: %1, error: %2}")
                      .arg(ai->m4aURL64(), err);
    emit DownloadFailed(trackID_, dlItemData_->number(), err);
    delete err;
  } else
    emit DownloadSucceed(trackID_, dlItemData_->number());
}

TrackInfo *DownloadChargeTrackRunnable::GetChargeTrackInfo(int trackID) {
  auto dataErr = CgoGetVipAudioInfo(
      trackID_, const_cast<char *>(cookie_.toStdString().c_str()));

  if (dataErr->error) {
    emit GetInfoFailed(trackID_, QString(dataErr->error), dlItemData_);
    delete dataErr;
    return nullptr;
  }

  auto cgo = static_cast<CgoTrackInfo *>(dataErr->data);
  auto ai = new TrackInfo(cgo->id, cgo->title, cgo->duration, cgo->mp3URL32,
                          cgo->mp3URL64, cgo->m4aURL24, cgo->m4aURL64);
  //  emit Finished(ai, dlItemData_);
  delete cgo;
  delete dataErr;
  return ai;
}
