#ifndef CGO_H
#define CGO_H

#include <QMetaType>

struct AlbumInfo {
  const char* title;
  int audioCount;
  int pageCount;
  const char* error;

  ~AlbumInfo() {
    delete[] title;
    delete[] error;
  }
};

struct AudioItem {
  int id;
  const char* title;
  const char* url;
  ~AudioItem() {
    delete title;
    delete url;
  }
};
Q_DECLARE_METATYPE(AudioItem*)

struct Array {
  void** pointer = nullptr;
  int length = 0;
};

struct DataError {
  void* data;
  const char* error;

  ~DataError() { delete error; }
};

typedef AlbumInfo* (*CGO_GET_ALBUM_INFO)(int albumID);
typedef DataError* (*CGO_GET_AUDIO_INFO)(int albumID, int page, int pageSize);
typedef DataError* (*CGO_GET_VIP_AUDIO_INFO)(int albumID, const char* cookie);

typedef const char* (*CGO_DOWNLOAD_FILE)(const char* url, const char* filePath,
                                         int id);
typedef struct DataError* (*CGO_GET_FILE_LENGTH)(const char* url);

class Cgo {
 private:
  Cgo();

 public:
  static Cgo* getInstance();
  int setCgo(const QString& funcName, void* funcPtr);

 public:
  CGO_GET_ALBUM_INFO cgo_getAlbumInfo = nullptr;
  CGO_GET_AUDIO_INFO cgo_getAudioInfo = nullptr;
  CGO_DOWNLOAD_FILE cgo_downloadFile = nullptr;
  CGO_GET_VIP_AUDIO_INFO cgo_getVipAudioInfo = nullptr;
  CGO_GET_FILE_LENGTH cgo_getFileLength = nullptr;
};

#endif  // CGO_H
