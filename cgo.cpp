#include "cgo.h"

#include <QObject>

Cgo::Cgo() {}

Cgo *Cgo::Cgo::getInstance() {
  static Cgo cgo;
  return &cgo;
}

int Cgo::setCgo(const QString &funcName, void *funcPtr) {
  if (funcName == QStringLiteral("cgo_getAlbumInfo"))
    cgo_getAlbumInfo = (CGO_GET_ALBUM_INFO)funcPtr;
  else if (funcName == QStringLiteral("cgo_getAudioInfo"))
    cgo_getAudioInfo = (CGO_GET_AUDIO_INFO)funcPtr;
  else if (funcName == QLatin1String("cgo_getVipAudioInfo"))
    cgo_getVipAudioInfo = (CGO_GET_VIP_AUDIO_INFO)funcPtr;
  else if (funcName == QStringLiteral("cgo_downloadFile"))
    cgo_downloadFile = (CGO_DOWNLOAD_FILE)funcPtr;
  else if (funcName == QStringLiteral("cgo_getUserInfo"))
    cgo_getUserInfo = (CGO_GET_USER_INFO)funcPtr;
  else if (funcName == QStringLiteral("cgo_readConfig"))
    cgo_readConfig = (CGO_READ_CONFIG)funcPtr;
  else if (funcName == QStringLiteral("cgo_writeConfig"))
    cgo_writeConfig = (CGO_WRITE_CONFIG)funcPtr;

  //  } else if (funcName == QStringLiteral("cgo_getFileLength"))
  //    cgo_getFileLength = (CGO_GET_FILE_LENGTH)funcPtr;

  return 1;
}
