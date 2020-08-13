package main

import "C"
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
	"unsafe"
)

//#cgo LDFLAGS: -lstdc++
//#include "cgo.h"
import "C"

var fileNameRegexp = regexp.MustCompile("[/\\:*?\"<>|]")

func main() {
	log.SetFlags(log.Lshortfile | log.Ldate | log.Ltime)

	//runtime.LockOSThread()
	//C.init()
	//C.callback()
	//C.start()
	//runtime.UnlockOSThread()
}

var uflCallback C.UpdateFileLengthCallback

//export CgoRegisterCallback
func CgoRegisterCallback(callback C.UpdateFileLengthCallback) {
	uflCallback = callback
}

//export CgoGetAlbumInfo
func CgoGetAlbumInfo(albumID C.int) *C.DataError {
	title, audioCount, pageCount, err := xmlydown.GetAlbumInfo(int(albumID))
	if err != nil {
		return C.newDataError(nil, C.CString(err.Error()))
	}

	pAlbumInfo := C.newAlbumInfo(C.CString(title), C.int(audioCount), C.int(pageCount))
	return C.newData(unsafe.Pointer(pAlbumInfo))
}

//export CgoGetAudioInfo
func CgoGetAudioInfo(albumID, page, pageSize C.int) *C.DataError {
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

//export CgoGetVipAudioInfo
func CgoGetVipAudioInfo(trackID C.int, cookie *C.char) *C.DataError {
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

//export CgoDownloadFile
func CgoDownloadFile(cUrl, cFilePath *C.char, id C.int) *C.char {
	var url string = C.GoString(cUrl)
	var filePath string = C.GoString(cFilePath)

	resp, err := client.Head(url)
	if err != nil {
		return C.CString(err.Error())
	}
	defer resp.Body.Close()

	contentLength := C.long(resp.ContentLength)
	currentLength := C.long(0)
	C.UpdateFileLength(uflCallback, id, &contentLength, &currentLength)
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
		currentLength = C.long(downloadProgress.CurrentLength)
		//fmt.Println("1")
		C.UpdateFileLength(uflCallback, id, &contentLength, &currentLength)
		//fmt.Println("2")
	}
	_, err = io.Copy(io.MultiWriter(file, downloadProgress), resp.Body)
	if err != nil {
		return C.CString(fmt.Sprintf("io copy %s fail: %s", filePath, err.Error()))
	}

	return nil
}

//export CgoGetUserInfo
func CgoGetUserInfo(cookie *C.char) *C.DataError {
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

//export CgoReadConfig
func CgoReadConfig() *C.AppConfig {
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

	return C.NewAppConfig(C.int(cfg.AlbumID), C.int(cfg.MaxTaskCount), C.int(cfg.Theme),
		C.CString(cfg.Cookie), C.CString(cfg.DownloadDir))
}

//export CgoWriteConfig
func CgoWriteConfig(albumID, maxTaskCount, theme C.int, cookie, downloadDir *C.char) {
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
