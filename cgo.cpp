#include "cgo.h"

#include <QObject>

Cgo::Cgo() {}

Cgo *Cgo::Cgo::getInstance() {
  static Cgo cgo;
  return &cgo;
}

int Cgo::setCgo(const QString &funcName, void *funcPtr) {
  if (funcName == QStringLiteral("cgo_getAudiobookInfo"))
    cgo_getAudiobookInfo = (CGO_GET_AUDIOBOOK_INFO)funcPtr;
  else if (funcName == QStringLiteral("cgo_getAudioInfo"))
    cgo_getAudioInfo = (CGO_GET_AUDIO_INFO)funcPtr;
  else if (funcName == QStringLiteral("cgo_downloadFile"))
    cgo_downloadFile = (CGO_DOWNLOAD_FILE)funcPtr;
  else if (funcName == QStringLiteral("cgo_getFileLength"))
    cgo_getFileLength = (CGO_GET_FILE_LENGTH)funcPtr;

  return 1;
}
