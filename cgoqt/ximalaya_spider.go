package main

import "C"
import (
	"errors"
	"fmt"
	"github.com/PuerkitoBio/goquery"
	jsoniter "github.com/json-iterator/go"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"regexp"
	"runtime"
	"strconv"
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
	} AudiobookInfo;

	typedef struct
	{
		int id;
		char* title;
		char* url;
	} AudioItem;

  	extern void init();
   	extern int start();
   	extern void drv_cgo_callback(void*, void*);
    extern void updateFileLength(int id, long *contentLength, long *currentLength);

   	static inline void callback() // GO中调用
   	{
		char* _cgo_getAudiobookInfo = "cgo_getAudiobookInfo";
		char* _cgo_getAudioInfo = "cgo_getAudioInfo";
		char* _cgo_downloadFile = "cgo_downloadFile";
		char* _cgo_getFileLength = "cgo_getFileLength";

		extern void* cgoGetAudiobookInfo(int audiobookId);
		extern DataError* cgoGetAudioInfo(int audiobookId, int page, int pageSize);
		extern char* cgoDownloadFile(char* url, char* filePath, int id);
		extern DataError* cgoGetFileLength(char* url);

		drv_cgo_callback(_cgo_getAudiobookInfo, &cgoGetAudiobookInfo);
		drv_cgo_callback(_cgo_getAudioInfo, &cgoGetAudioInfo);
		drv_cgo_callback(_cgo_downloadFile, &cgoDownloadFile);
		drv_cgo_callback(_cgo_getFileLength, &cgoGetFileLength);
   	}

	static inline void setAudiobookInfo(void ** pp, char* title, int audioCount, int pageCount)
	{
		AudiobookInfo *p = (AudiobookInfo*)malloc(sizeof(AudiobookInfo));
		memset(p, 0, sizeof(p));
		p->title = title;
		p->audioCount = audioCount;
		p->pageCount = pageCount;
		p->error = NULL;
		*pp = p;
	}

	static inline void setAudiobookInfoErr(void **pp, char* error)
	{
		AudiobookInfo *p = (AudiobookInfo*)malloc(sizeof(AudiobookInfo));
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

func main() {
	log.SetFlags(log.Lshortfile | log.Ldate | log.Ltime)

	runtime.LockOSThread()
	C.init()
	C.callback()
	C.start()
	runtime.UnlockOSThread()
}

//export cgoGetAudiobookInfo
func cgoGetAudiobookInfo(audiobookId C.int) unsafe.Pointer {
	var pAudiobookInfo unsafe.Pointer
	title, audioCount, pageCount, err := GetAudioBookInfo(int(audiobookId))
	if err != nil {
		C.setAudiobookInfoErr(&pAudiobookInfo, C.CString(err.Error()))
		return pAudiobookInfo
	}
	C.setAudiobookInfo(&pAudiobookInfo, C.CString(title), C.int(audioCount), C.int(pageCount))

	return pAudiobookInfo
}

//export cgoGetAudioInfo
func cgoGetAudioInfo(audiobookId, page, pageSize C.int) *C.DataError {
	list, err := GetAudioInfo(int(audiobookId), int(page), int(pageSize))
	if err != nil {
		return C.newDataError(nil, C.CString(err.Error()))
	}

	ptrArray := C.newPointerArray(C.int(len(list)))
	for i, v := range list {
		C.setPointerArray(ptrArray, C.int(i), C.newAudioItem(C.int(v.Id), C.CString(v.Title), C.CString(v.Url)))
	}
	cArray := C.newCArray(ptrArray, C.int(len(list)))

	p := C.newDataError(unsafe.Pointer(cArray), C.CString(""))
	return p
}

type DownloadProgress struct {
	io.Reader
	ContentLength, CurrentLength int64
	Progress func()
}

func (d *DownloadProgress) Write(p []byte) (n int, err error) {
	n = len(p)
	d.CurrentLength += int64(n)
	d.Progress()
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
	//currentLength := C.long(0)
	C.updateFileLength(id, &contentLength, new(C.long))

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
	downloadProgress.Progress = func() {
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

var client = http.Client{}

//export cgoGetFileLength
func cgoGetFileLength(cUrl *C.char) *C.DataError {
	var url string = C.GoString(cUrl)

	request, err := http.NewRequest("HEAD", url, nil)
	if err != nil {
		return C.newDataError(nil, C.CString(err.Error()))
	}

	resp, err := client.Do(request)
	if err != nil {
		return C.newDataError(nil, C.CString(err.Error()))
	}
	defer resp.Body.Close()

	cLong := C.long(resp.ContentLength)
	return C.newDataError(unsafe.Pointer(&cLong), C.CString(""))
}

type AudioItem struct {
	Id         int
	Title, Url string
}

func GetAudioInfo(audiobookId, page, pageSize int) (audioList []AudioItem, err error) {
	format := fmt.Sprintf("https://m.ximalaya.com/m-revision/common/album/queryAlbumTrackRecordsByPage?albumId=%d&page=%d&pageSize=%d&asc=true", audiobookId, page, pageSize)
	log.Println("Get:", format)

	resp, err := http.Get(format)
	if err != nil {
		return nil, fmt.Errorf("http get %v fail:%v", format, err.Error())
	}

	defer resp.Body.Close()
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return nil, err
	}


	list := jsoniter.Get(body, "data").Get("trackDetailInfos")
	for i2 := 0; i2 < list.Size(); i2++ {
		v := list.Get(i2).Get("trackInfo")
		audioList = append(audioList, AudioItem{Id: v.Get("id").ToInt(), Url: v.Get("playPath").ToString(), Title: v.Get("title").ToString()})
	}

	return audioList, nil
}

func GetAudioBookInfo(audiobookId int) (title string, audioCount, pageCount int, err error) {
	resp, err := http.Get(fmt.Sprintf("https://www.ximalaya.com/youshengshu/%d/", audiobookId))
	if err != nil {
		return "", -1, -1, errors.New(err.Error())
	}
	defer resp.Body.Close()
	if resp.StatusCode != 200 {
		return "", -1, -1, errors.New("get page fail : StatusCode != 200")
	}

	doc, err := goquery.NewDocumentFromReader(resp.Body)
	if err != nil {
		return "", -1, -1, fmt.Errorf("new goquery document fail:" + err.Error())
	}

	//标题
	title = doc.Find("h1.title").Text()

	//音频数量
	r, _ := regexp.Compile("\\d+\\.?\\d*")
	num := r.FindString(doc.Find("div.head").Text())
	audioCount, _ = strconv.Atoi(num)

	//页面数量
	list := doc.Find("ul.pagination-page").Children()
	size := list.Size()
	if size > 6 { //超过5页
		i, err := strconv.Atoi(list.Eq(list.Size() - 2).Text())
		if err != nil {
			log.Fatal(err)
		}
		pageCount = i
	} else if size == 0 { //就一页
		pageCount = 1
	} else { //大于0页 && 小于等于5页
		pageCount = size - 1 //1为下一页按钮
	}
	return
}

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
