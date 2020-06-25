package main

import (
	"fmt"
	xmlydown "github.com/jing332/xmlydownloader"
	jsoniter "github.com/json-iterator/go"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"regexp"
	"runtime"
	"unsafe"
)

/*
	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <string.h>

	typedef struct
	{
		void* pointer;
		int length;
	} CArray;

	typedef struct
	{
		void* data;
		const char* error;
	} DataError;

	typedef struct
	{
		char* title;
		int audioCount;
		int pageCount;
		char* error;
	} AlbumInfo;

	typedef struct
	{
		int id;
		char* title;
		char* url;
		char* number;
	} AudioItem;

	typedef struct
	{
		int ret;
		char* msg;

		int uid;
		int isVip;
		char* nickName;
	} UserInfo;

	typedef struct
	{
		int albumID;
		int maxTaskCount;
		int theme;
		const char* cookie;
		const char* downloadDir;
	} AppConfig;

  	extern void init();
   	extern int start();
   	extern void drv_cgo_callback(void*, void*);
    extern void updateFileLength(int id, long *contentLength, long *currentLength);

   	static inline void callback() // GO中调用
   	{
		char* _cgo_getAlbumInfo = "cgo_getAlbumInfo";
		char* _cgo_getAudioInfo = "cgo_getAudioInfo";
		char* _cgo_getVipAudioInfo = "cgo_getVipAudioInfo";
		char* _cgo_downloadFile = "cgo_downloadFile";
		char* _cgo_getUserInfo = "cgo_getUserInfo";
		char* _cgo_readConfig = "cgo_readConfig";
		char* _cgo_writeConfig = "cgo_writeConfig";
		//char* _cgo_getFileLength = "cgo_getFileLength";

		extern void* cgoGetAlbumInfo(int albumID);
		extern DataError* cgoGetAudioInfo(int albumID, int page, int pageSize);
		extern DataError* cgoGetVipAudioInfo(int trackID, char* cookie);
		extern char* cgoDownloadFile(char* url, char* filePath, int id);
		extern DataError* cgoGetUserInfo(char* cookie);

		extern AppConfig* cgoReadConfig();
		extern void cgoWriteConfig(int albumID,  int maxTaskCount, int theme, char* cookie, char* downloadDir);
		//extern DataError* cgoGetFileLength(char* url);

		drv_cgo_callback(_cgo_getAlbumInfo, &cgoGetAlbumInfo);
		drv_cgo_callback(_cgo_getAudioInfo, &cgoGetAudioInfo);
		drv_cgo_callback(_cgo_getVipAudioInfo, &cgoGetVipAudioInfo);
		drv_cgo_callback(_cgo_downloadFile, &cgoDownloadFile);
		drv_cgo_callback(_cgo_getUserInfo, &cgoGetUserInfo);
		drv_cgo_callback(_cgo_readConfig, &cgoReadConfig);
		drv_cgo_callback(_cgo_writeConfig, &cgoWriteConfig);
		//drv_cgo_callback(_cgo_getFileLength, &cgoGetFileLength);
   	}

	static inline void setAlbumInfo(void ** pp, char* title, int audioCount, int pageCount)
	{
		AlbumInfo *p = (AlbumInfo*)malloc(sizeof(AlbumInfo));
		memset(p, 0, sizeof(p));
		p->title = title;
		p->audioCount = audioCount;
		p->pageCount = pageCount;
		p->error = NULL;
		*pp = p;
	}

	static inline void setAlbumInfoErr(void **pp, char* error)
	{
		AlbumInfo *p = (AlbumInfo*)malloc(sizeof(AlbumInfo));
		memset(p, 0, sizeof(p));
		p->title = NULL;
		p->error = error;
		*pp = p;
	}

   	static inline void* newAudioItem(int id, char* title, char* url)
	{
		AudioItem *p = (AudioItem*)malloc(sizeof(AudioItem));
		memset(p, 0, sizeof(p));
		p->id = id;
		p->title = title;
		p->url = url;
		p->number = NULL;
		return p;
	}

	static inline void setAudioItem(void **pp, int id, char* title, char* url)
	{
		AudioItem *p = (AudioItem*)malloc(sizeof(AudioItem));
		memset(p, 0, sizeof(p));
		p->id = id;
		p->title = title;
		p->url = url;
		*pp = p;
	}

  	static inline void* newPointerArray(int length)
	{
		void** cArray = (void**)malloc(length * sizeof(void*));
		return cArray;
	}

 	static inline void setPointerArray(void* array, int index, void* v)
	{
		void** p = (void**)array;
		p[index] = v;
	}

	static inline CArray* newCArray(void* p, int length)
	{
		CArray *array = (CArray*)malloc(sizeof(CArray));
		array->pointer = p;
		array->length = length;
		return array;
	}

	static inline DataError* newDataError(void* data, const char* error)
	{
		DataError *dataError = (DataError*)malloc(sizeof(DataError));
		dataError->data = data;
		dataError->error = error;

		return dataError;
	}

	static inline DataError* newData(void* data)
	{
		DataError *dataError = (DataError*)malloc(sizeof(DataError));
		dataError->data = data;
		dataError->error = NULL;

		return dataError;
	}

    static inline void* newUserInfo(int ret, char* msg, int uid, int isVip, char* nickName)
	{
		UserInfo *p = (UserInfo*)malloc(sizeof(UserInfo));
		p->ret = ret;
		p->msg = msg;
		p->uid = uid;
		p->isVip = isVip;
		p->nickName = nickName;

		return p;
	}

	static inline AppConfig* newAppConfig(int albumID, int maxTaskCount, int theme, const char* cookie, const char* downloadDir)
	{
		AppConfig *p = (AppConfig*)malloc(sizeof(AppConfig));
		p->albumID = albumID;
		p->maxTaskCount = maxTaskCount;
		p->theme = theme;
		p->cookie = cookie;
		p->downloadDir = downloadDir;

		return p;
	}

	//static inline void setCArray(void **pp, void *pointer, int length)
	//{
	//	CArray *p = (CArray*)malloc(sizeof(CArray));
	//	memset(p, 0, sizeof(p));
	//	p->pointer = pointer;
	//	p->length = length;
	//	*pp = p;
	//}

*/
//#cgo LDFLAGS: -L./ -lxmly-downloader-qt5
import "C"

var fileNameRegexp = regexp.MustCompile("[\\\\/:*?\"<>|]")

func main() {
	log.SetFlags(log.Lshortfile | log.Ldate | log.Ltime)

	runtime.LockOSThread()
	C.init()
	C.callback()
	C.start()
	runtime.UnlockOSThread()
}

//export cgoGetAlbumInfo
func cgoGetAlbumInfo(albumID C.int) unsafe.Pointer {
	var pAlbumInfo unsafe.Pointer
	title, audioCount, pageCount, err := xmlydown.GetAlbumInfo(int(albumID))
	if err != nil {
		C.setAlbumInfoErr(&pAlbumInfo, C.CString(err.Error()))
		return pAlbumInfo
	}
	C.setAlbumInfo(&pAlbumInfo, C.CString(title), C.int(audioCount), C.int(pageCount))

	return pAlbumInfo
}

//export cgoGetAudioInfo
func cgoGetAudioInfo(albumID, page, pageSize C.int) *C.DataError {
	list, err := xmlydown.GetAudioInfo(int(albumID), int(page), int(pageSize))
	if err != nil {
		return C.newDataError(nil, C.CString(err.Error()))
	}

	ptrArray := C.newPointerArray(C.int(len(list)))
	for i, v := range list {
		//将特殊字符替换为空格
		v.Title = fileNameRegexp.ReplaceAllLiteralString(v.Title, " ")
		C.setPointerArray(ptrArray, C.int(i), C.newAudioItem(C.int(v.TrackId), C.CString(v.Title), C.CString(v.URL)))
	}
	cArray := C.newCArray(ptrArray, C.int(len(list)))

	p := C.newData(unsafe.Pointer(cArray))
	return p
}

//export cgoGetVipAudioInfo
func cgoGetVipAudioInfo(trackID C.int, cookie *C.char) *C.DataError {
	ai, err := xmlydown.GetVipAudioInfo(int(trackID), C.GoString(cookie))
	if err != nil {
		return C.newDataError(nil, C.CString(err.Error()))
	}
	//将特殊字符替换为空格
	ai.Title = fileNameRegexp.ReplaceAllLiteralString(ai.Title, " ")

	return C.newData(C.newAudioItem(C.int(ai.TrackId), C.CString(ai.Title), C.CString(ai.URL)))
}

//DownloadProgress 追踪下载进度
type DownloadProgress struct {
	io.Reader
	ContentLength, CurrentLength int64
	ProgressEvent                func()
}

func (d *DownloadProgress) Write(p []byte) (n int, err error) {
	n = len(p)
	d.CurrentLength += int64(n)
	d.ProgressEvent()
	return
}

//export cgoDownloadFile
func cgoDownloadFile(cUrl, cFilePath *C.char, id C.int) *C.char {
	var url string = C.GoString(cUrl)
	var filePath string = C.GoString(cFilePath)

	resp, err := client.Head(url)
	if err != nil {
		return C.CString(err.Error())
	}
	defer resp.Body.Close()

	contentLength := C.long(resp.ContentLength)
	C.updateFileLength(id, &contentLength, new(C.long))
	//判断同名文件是否存在并对比大小
	if fileInfo, err := os.Stat(filePath); err == nil {
		if fileInfo.Size() == resp.ContentLength {
			return nil
		}
	}

	resp, err = httpGet(url)
	if err != nil {
		return C.CString(fmt.Sprintf("download %s fail: %s", url, err.Error()))
	}
	defer resp.Body.Close()
	if resp.StatusCode != 200 {
		return C.CString("download" + url + "fail: StatusCode != 200")
	}

	//目录不存在则创建
	err = os.MkdirAll(filepath.Dir(filePath), 0777)
	if err != nil && !os.IsExist(err) {
		return C.CString(fmt.Sprintf("make dir fail: %s", err.Error()))
	}

	//创建并写入文件
	file, err := os.Create(filePath)
	if err != nil {
		return C.CString(fmt.Sprintf("create file %s fail: %s", filePath, err.Error()))
	}
	defer file.Close()

	downloadProgress := &DownloadProgress{ContentLength: resp.ContentLength}
	downloadProgress.ProgressEvent = func() {
		contentLength = C.long(downloadProgress.ContentLength)
		currentLength := C.long(downloadProgress.CurrentLength)
		C.updateFileLength(id, &contentLength, &currentLength)
	}
	_, err = io.Copy(io.MultiWriter(file, downloadProgress), resp.Body)
	if err != nil {
		return C.CString(fmt.Sprintf("io copy %s fail: %s", filePath, err.Error()))
	}

	return nil
}

//export cgoGetUserInfo
func cgoGetUserInfo(cookie *C.char) *C.DataError {
	ui, err := xmlydown.GetUserInfo(C.GoString(cookie))
	if err != nil {
		return C.newDataError(nil, C.CString(err.Error()))
	}

	isVip := 0
	if ui.Data.IsVip {
		isVip = 1
	}
	var p unsafe.Pointer
	p = C.newUserInfo(C.int(ui.Ret), C.CString(ui.Msg), C.int(ui.Data.UID), C.int(isVip), C.CString(ui.Data.Nickname))

	return C.newData(p)
}

//AppConfig 应用配置
type AppConfig struct {
	AlbumID      int    `json:"albumID"`
	MaxTaskCount int    `json:"maxTaskCount"`
	Theme        int    `json:"theme"`
	Cookie       string `json:"cookie"`
	DownloadDir  string `json:"downloadDir"`
}

//export cgoReadConfig
func cgoReadConfig() *C.AppConfig {
	f, err := os.Open("config.json")
	if err != nil {
		return nil
	}
	defer f.Close()

	data, err := ioutil.ReadAll(f)
	if err != nil {
		return nil
	}

	var cfg AppConfig
	err = jsoniter.Unmarshal(data, &cfg)
	if err != nil {
		return nil
	}

	return C.newAppConfig(C.int(cfg.AlbumID), C.int(cfg.MaxTaskCount), C.int(cfg.Theme),
		C.CString(cfg.Cookie), C.CString(cfg.DownloadDir))
}

//export cgoWriteConfig
func cgoWriteConfig(albumID, maxTaskCount, theme C.int, cookie, downloadDir *C.char) {
	f, err := os.OpenFile("config.json", os.O_WRONLY|os.O_CREATE|os.O_TRUNC, 0666)
	if err != nil {
		return
	}
	defer f.Close()

	appCfg := AppConfig{AlbumID: int(albumID), MaxTaskCount: int(maxTaskCount), Theme: int(theme),
		Cookie: C.GoString(cookie), DownloadDir: C.GoString(downloadDir)}
	json, err := jsoniter.Marshal(appCfg)
	if err != nil {
		return
	}
	f.Write(json)
}

////export cgoGetFileLength
//func cgoGetFileLength(cUrl *C.char) *C.DataError {
//	var url string = C.GoString(cUrl)
//
//	request, err := http.NewRequest("HEAD", url, nil)
//	if err != nil {
//		return C.newDataError(nil, C.CString(err.Error()))
//	}
//
//	resp, err := client.Do(request)
//	if err != nil {
//		return C.newDataError(nil, C.CString(err.Error()))
//	}
//	defer resp.Body.Close()
//
//	cLong := C.long(resp.ContentLength)
//	return C.newDataError(unsafe.Pointer(&cLong), C.CString(""))
//}

var client = http.Client{}

func httpGet(url string) (*http.Response, error) {
	req, err := http.NewRequest("GET", url, nil)
	if err != nil {
		return nil, err
	}

	req.Header.Set("Connection", "keep-alive")
	req.Header.Set("Accept-Language", "zh-CN,zh;q=0.9")
	req.Header.Set("user-agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/84.0.4139.0 Safari/537.36 Edg/84.0.516.1")

	return client.Do(req)
}
