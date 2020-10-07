#ifndef CGO_H
#define CGO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  void* pointer;
  int length;
} CArray;

typedef struct {
  void* data;
  const char* error;
} DataError;

typedef struct {
  char* title;
  int trackCount;
  int type;
  char* freeTrackIDs;
} AlbumInfo;

typedef struct {
  int id;
  char* title;
  int duration;
  char* mp3URL32;
  char* mp3URL64;
  char* m4aURL24;
  char* m4aURL64;
} CgoTrackInfo;

typedef struct {
 int maxPageID;
 void* list;
} CgoTrackList;

typedef struct {
  int ret;
  char* msg;
  int uid;
  int isVip;
  char* nickName;
} UserInfo;

typedef struct {
  int ret;
  char* msg;
  char* qrID;
  char* img;
} CgoQRCode;

typedef void (*UpdateFileLengthCallback)(int trackID, long* contentLength,
                                         long* currentLength);

static void UpdateFileLength(UpdateFileLengthCallback callback, int trackID,
                             long* contentLength, long* currentLength) {
  callback(trackID, contentLength, currentLength);
}

static inline AlbumInfo* newAlbumInfo(char* title, int trackCount,
                                      int type, char* freeTrackIDs) {
  AlbumInfo* p = (AlbumInfo*)malloc(sizeof(AlbumInfo));
  memset(p, 0, sizeof(AlbumInfo));
  p->title = title;
  p->trackCount = trackCount;
  p->type = type;
  p->freeTrackIDs = freeTrackIDs;
  return p;
}

static inline void* newTrackInfo(int id, char* title, int duration, char* mp3URL32,
                                 char* mp3URL64, char* m4aUURL24,
                                 char* m4aURL64) {
  CgoTrackInfo* p = (CgoTrackInfo*)malloc(sizeof(CgoTrackInfo));
  memset(p, 0, sizeof(CgoTrackInfo));
  p->id = id;
  p->title = title;
  p->duration = duration;
  p->mp3URL32 = mp3URL32;
  p->mp3URL64 = mp3URL64;
  p->m4aURL24 = m4aUURL24;
  p->m4aURL64 = m4aURL64;
  return p;
}

static inline void* newTrackList(int maxPageID, void* list){
  CgoTrackList *p = (CgoTrackList*)malloc(sizeof (CgoTrackList));
  memset(p, 0, sizeof (CgoTrackList));
  p->maxPageID = maxPageID;
  p->list = list;
  return p;
}

static inline void* newPointerArray(int length) {
  void** cArray = (void**)malloc(length * sizeof(void*));
  memset(cArray, 0, sizeof(length * sizeof(void*)));
  return cArray;
}

static inline void setPointerArray(void* array, int index, void* v) {
  void** p = (void**)array;
  p[index] = v;
}

static inline CArray* newCArray(void* p, int length) {
  CArray* array = (CArray*)malloc(sizeof(CArray));
  memset(array, 0, sizeof(CArray));
  array->pointer = p;
  array->length = length;
  return array;
}

static inline DataError* newDataError(void* data, const char* error) {
  DataError* p = (DataError*)malloc(sizeof(DataError));
  memset(p, 0, sizeof(DataError));
  p->data = data;
  p->error = error;
  return p;
}

static inline DataError* newData(void* data) {
  DataError* p = (DataError*)malloc(sizeof(DataError));
  memset(p, 0, sizeof(DataError));
  p->data = data;
  p->error = NULL;
  return p;
}

static inline void* newUserInfo(int ret, char* msg, int uid, int isVip,
                                char* nickName) {
  UserInfo* p = (UserInfo*)malloc(sizeof(UserInfo));
  memset(p, 0, sizeof(UserInfo));
  p->ret = ret;
  p->msg = msg;
  p->uid = uid;
  p->isVip = isVip;
  p->nickName = nickName;
  return p;
}

static inline void* newQRCode(int ret, char* msg, char* qrID, char *img){
  CgoQRCode *p = (CgoQRCode*)malloc(sizeof(CgoQRCode));
  memset(p, 0, sizeof(CgoQRCode));
  p->ret = ret;
  p->msg = msg;
  p->qrID = qrID;
  p->img = img;
  return p;
}

// static inline void setCArray(void **pp, void *pointer, int length)
//{
//	CArray *p = (CArray*)malloc(sizeof(CArray));
//	memset(p, 0, sizeof(p));
//	p->pointer = pointer;
//	p->length = length;
//	*pp = p;
//}

// static inline void setAudioItem(void** pp, int id, char* title, char* url) {
//  CgoAudioItem* p = (CgoAudioItem*)malloc(sizeof(CgoAudioItem));
//  memset(p, 0, sizeof(CgoAudioItem));
//  p->id = id;
//  p->title = title;
//  p->url = url;
//  *pp = p;
//}

// static inline void setAlbumInfoErr(void** pp, char* error) {
//  AlbumInfo* p = (AlbumInfo*)malloc(sizeof(AlbumInfo));
//  memset(p, 0, sizeof(AlbumInfo));
//  p->title = NULL;
//  *pp = p;
//}

// static inline void setAlbumInfo(void** pp, char* title, int audioCount,
//                                int pageCount) {
//  AlbumInfo* p = (AlbumInfo*)malloc(sizeof(AlbumInfo));
//  memset(p, 0, sizeof(AlbumInfo));
//  p->title = title;
//  p->audioCount = audioCount;
//  p->pageCount = pageCount;
//  *pp = p;
//}

#endif  // CGO_H
