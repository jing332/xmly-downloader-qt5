#ifndef CGO_H
#define CGO_H

#include <QMetaType>

struct AudioBookInfo {
  const char* title;
  int audioCount;
  int pageCount;
  const char* error;

  ~AudioBookInfo() {
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

typedef struct AudioBookInfo* (*CGO_GET_AUDIOBOOK_INFO)(int audiobookId);
typedef struct DataError* (*CGO_GET_AUDIO_INFO)(int audiobookId, int page,
                                                int pageSize);

typedef const char* (*CGO_DOWNLOAD_FILE)(const char* url, const char* filePath,
                                         int id);
typedef struct DataError* (*CGO_GET_FILE_LENGTH)(const char* url);

class Cgo {
 private:
  Cgo();

 public:
  static Cgo* getInstance();
  int setCgo(const QString& funcName, void* _b);

 public:
  CGO_GET_AUDIOBOOK_INFO cgo_getAudiobookInfo = nullptr;
  CGO_GET_AUDIO_INFO cgo_getAudioInfo = nullptr;
  CGO_DOWNLOAD_FILE cgo_downloadFile = nullptr;
  CGO_GET_FILE_LENGTH cgo_getFileLength = nullptr;
};

#endif  // CGO_H
