#include "cgo.h"

#include <QObject>

Cgo::Cgo() {}

Cgo *Cgo::Cgo::getInstance() {
  static Cgo cgo;
  return &cgo;
}

int Cgo::setCgo(void *_a, void *_b) {
  if (QStringLiteral("cgo_getAudiobookInfo").compare((char *)_a) == 0)
    cgo_getAudiobookInfo = (CGO_GET_AUDIOBOOK_INFO)_b;

  if (QStringLiteral("cgo_getAudioInfo").compare((char *)_a) == 0)
    cgo_getAudioInfo = (CGO_GET_AUDIO_INFO)_b;

  if (QStringLiteral("cgo_downloadFile").compare((char *)_a) == 0)
    cgo_downloadFile = (CGO_DOWNLOAD_FILE)_b;

  if (QStringLiteral("cgo_getFileLength").compare((char *)_a) == 0)
    cgo_getFileLength = (CGO_GET_FILE_LENGTH)_b;

  return 1;
}
