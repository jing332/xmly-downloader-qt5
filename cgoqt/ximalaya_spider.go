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
	extern void addAudioItem(void*);

   	static void callback() // GO中调用
   	{
		char* _cgo_getAudiobookInfo = "cgo_getAudiobookInfo";
		//char* _cgo_getAudioList = "cgo_getAudioList";
		char* _cgo_getAudioInfo = "cgo_getAudioInfo";
		char* _cgo_downloadFile = "cgo_downloadFile";
		char* _cgo_getFileLength = "cgo_getFileLength";

		extern void* cgoGetAudiobookInfo(int audiobookId);
		//extern void* cgoGetAudioList(int audiobookId, int audioCount);
		extern DataError* cgoGetAudioInfo(int audiobookId, int page, int pageSize);
		extern char* cgoDownloadFile(char* url, char* filePath);
		extern DataError* cgoGetFileLength(char* url);

		//drv_cgo_callback(_cgo_getAudioList, &cgoGetAudioList);
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

	static inline void printArray(void *array, int index)
	{
		void** p = (void**)array;
		printf("printArray: %x %d\n",  p[index], sizeof(array));
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
		//printf("arrayPointer: %d %d, arrayIndex: %d\n", array, p, v);
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

	//static inline long* newCLong(long l)
	//{
	//	long* p = (long*)
	//}

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

	//cArray := C.newPointerArray(2)
	//C.setPointerArray(cArray, 0, C.newAudioItem(C.int(00), C.CString("Title0"), C.CString("URL0")))
	//C.setPointerArray(cArray, 1, C.newAudioItem(C.int(11), C.CString("Title1"), C.CString("URL1")))
	//C.printArray(cArray, 0)
	//C.printArray(cArray, 1)

	log.Println("start")
	runtime.LockOSThread()
	C.init()
	C.callback()
	C.start()
	runtime.UnlockOSThread()
	log.Println("exit")
}

//export cgoGetAudiobookInfo
func cgoGetAudiobookInfo(audiobookId C.int) unsafe.Pointer {
	var pAudiobookInfo unsafe.Pointer
	title, audioCount, pageCount, err := GetAudioBookInfo(int(audiobookId))
	if err != nil {
		log.Println(err)
		C.setAudiobookInfoErr(&pAudiobookInfo, C.CString(err.Error()))
		return pAudiobookInfo
	}
	C.setAudiobookInfo(&pAudiobookInfo, C.CString(title), C.int(audioCount), C.int(pageCount))

	return pAudiobookInfo
}

////export cgoGetAudioList
//func cgoGetAudioList(audiobookId, audioCount C.int) (cArray unsafe.Pointer) {
//	//list := GetAudioInfoList(int(audiobookId), int(audioCount))
//
//	//pList := *(*[]byte)(unsafe.Pointer(&list))
//	//C.setCArray(&cArray, C.CBytes(pList), C.int(len(list)))
//
//	return
//}

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

//export cgoDownloadFile
func cgoDownloadFile(cUrl, cFilePath *C.char) *C.char {
	var url string = C.GoString(cUrl)
	var filePath string = C.GoString(cFilePath)

	resp, err := http.Get(url)
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

	_, err = io.Copy(file, resp.Body)
	if err != nil {
		return C.CString(fmt.Sprintf("io copy %s fail: %s", filePath, err.Error()))
	}

	return nil
}

//export cgoGetFileLength
func cgoGetFileLength(cUrl *C.char) *C.DataError {
	var url string = C.GoString(cUrl)

	request,err := http.NewRequest("HEAD", url, nil)
	if err != nil {
		return C.newDataError(nil, C.CString(err.Error()))
	}

	client := &http.Client{}
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

	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		return nil, err
	}
	resp.Body.Close()

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

//

//func Start(audiobookId, begin, end, maxRoutineCount int, downloadDir string) error {
//	//获取有声小说信息
//	title, audioCount, _, err := GetAudioBookInfo(audiobookId)
//	if err != nil {
//		return err
//	}
//	downloadDir += "/" + title
//
//	ch := make(chan int, maxRoutineCount)
//	var wg sync.WaitGroup
//
//	//获取所有音频
//	list := GetAudioInfoList(audiobookId, audioCount)
//	for i, v := range list {
//		i++
//		if i >= begin && (i <= end || end == -1) {
//			ch <- 1
//			wg.Add(1)
//
//			go func(url, dir, fileName string, index int, ch chan int) {
//				log.Println("download:", index, fileName, url)
//
//				//去除非法字符
//				reg, _ := regexp.Compile("[\\\\/:*?\"<>|]")
//				fileName = reg.ReplaceAllString(fileName, "")
//
//				err = download(url, fmt.Sprintf("%s/%s.m4a", dir, fileName), true, ch)
//				if err != nil {
//					log.Fatal(err)
//				}
//				wg.Add(-1)
//			}(v.Url, downloadDir, v.Title, i, ch)
//		}
//	}
//
//	wg.Wait()
//	return nil
//}

//func GetAudioInfoList(audiobookId, audioCount int) (audioList []AudioItem) {
//	if audioCount > 100 {
//		//分number次Get
//		number := audioCount / 100
//		for i := 0; i < number; i++ {
//			list, err := GetAudioInfo(audiobookId, i+1, 100)
//			if err != nil {
//				log.Fatal(err)
//			}
//			audioList = append(audioList, list...)
//		}
//
//		j := audioCount % 100
//		if j > 0 { //有余数
//			list, err := GetAudioInfo(audiobookId, number+1, j)
//			if err != nil {
//				log.Fatal(err)
//			}
//			audioList = append(audioList, list...)
//		}
//	} else { //音频数量 <= 100
//		list, err := GetAudioInfo(audiobookId, 1, audioCount)
//		if err != nil {
//			log.Fatal(err)
//		}
//		audioList = append(audioList, list...)
//	}
//
//	return audioList
//}

//func download(url, filePath string, forceSave bool, ch chan int) error {
//	if !forceSave {
//		if _, err := os.Stat(filePath); err == nil {
//			log.Println("file path exists:", filePath)
//			return nil
//		}
//	}
//
//	resp, err := http.Get(url)
//	if err != nil {
//		log.Printf("download %s fail: %s", url, err.Error())
//		return download(url, filePath, forceSave, ch)
//	}
//	defer resp.Body.Close()
//	if resp.StatusCode != 200 {
//		log.Fatal("download" + url + "fail: StatusCode != 200")
//	}
//
//	//目录不存在则创建
//	err = os.MkdirAll(filepath.Dir(filePath), 0777)
//	if err != nil && !os.IsExist(err) {
//		return fmt.Errorf("make dir fail: %s", err.Error())
//	}
//
//	//创建并写入文件
//	file, err := os.Create(filePath)
//	if err != nil {
//		return fmt.Errorf("create file %s fail: %s", filePath, err.Error())
//	}
//	defer file.Close()
//
//	_, err = io.Copy(file, resp.Body)
//	if err != nil {
//		return fmt.Errorf("io copy %s fail: %s", filePath, err.Error())
//	}
//
//	<-ch
//	return nil
//}

//var (
//	mainPageId  = 19383749 //https://www.ximalaya.com/youshengshu/19383749/
//	downloadDir = "download"
//)
//func main() {
//	log.SetFlags(log.Ltime | log.Lshortfile)
//
//	//mainPageId:有声小说主页Id, begin: 起始音频, end: 结束音频, maxRoutineCount: 最大协程(建议1-5 太大可能导致磁盘写入跟不上), downloadDir: 下载目录
//	err := Start(mainPageId, 1, -1, 5, downloadDir)
//	if err != nil {
//  log.Fatal(err)
//	}
//}