#include "downloadvipfilerunnable.h"

#include "cgoqt/xmlydownloader.h"

DownloadVipFileRunnable::DownloadVipFileRunnable(int trackID,
                                                 const QString &cookie,
                                                 const QString &filePath,
                                                 AudioItem *ai)
    : trackID_(trackID), cookie_(cookie), filePath_(filePath), ai_(ai) {}

void DownloadVipFileRunnable::run() {
  /*获取Vip音频信息*/
  emit StartGetInfo(trackID_);
  auto dataErr = CgoGetVipAudioInfo(
      trackID_, const_cast<char *>(cookie_.toStdString().c_str()));
  if (dataErr->error) {
    emit GetInfoError(trackID_, ai_, dataErr->error);
    return;
  }
  auto tmpAi = static_cast<CgoAudioItem *>(dataErr->data);
  emit StartDownload(trackID_, ai_);

  /*开始下载文件*/
  auto *cstr = CgoDownloadFile(
      const_cast<char *>(tmpAi->url),
      const_cast<char *>(filePath_.toStdString().c_str()), tmpAi->id);

  if (cstr) {
    emit DownloadError(trackID_, ai_, QString(cstr));
    delete cstr;
  } else {
    emit Finished(trackID_);
  }
}
