package main

import "C"
import (
	"fmt"
	xmly "github.com/jing332/xmlydownloader"
	"io"
	"log"
	"net/http"
	"os"
	"path/filepath"
	"regexp"
	"strings"
	"unsafe"
)

//#include "cgo.h"
import "C"

var fileNameRegexp = regexp.MustCompile("[/\\:*?\"<>|]")

func main() {
	log.SetFlags(log.Lshortfile | log.Ltime)
}

var uflCallback C.UpdateFileLengthCallback

//export CgoRegisterCallback
func CgoRegisterCallback(callback C.UpdateFileLengthCallback) {
	uflCallback = callback
}

//export CgoGetAlbumInfo
func CgoGetAlbumInfo(albumID C.int) *C.DataError {
	title, audioCount, pageCount, err := xmly.GetAlbumInfo(int(albumID))
	if err != nil {
		return C.newDataError(nil, C.CString(err.Error()))
	}

	pAlbumInfo := C.newAlbumInfo(C.CString(title), C.int(audioCount), C.int(pageCount))
	return C.newData(unsafe.Pointer(pAlbumInfo))
}

//export CgoGetAudioInfoListByPageID
func CgoGetAudioInfoListByPageID(albumID, pageID C.int) *C.DataError {
	playlist, err := xmly.GetAudioInfoListByPageID(int(albumID), int(pageID))
	if err != nil {
		return C.newDataError(nil, C.CString(err.Error()))
	}

	ptrArray := C.newPointerArray(C.int(len(playlist.List)))
	for i, v := range playlist.List {
		v.Title = fileNameRegexp.ReplaceAllLiteralString(v.Title, " ")
		v.Title = strings.ReplaceAll(v.Title, "\t", "")
		v.Title = strings.TrimSpace(v.Title)
		C.setPointerArray(ptrArray, C.int(i), C.newAudioItem(C.int(v.TrackID), C.CString(v.Title),
			C.CString(v.PlayURL32), C.CString(v.PlayURL64), C.CString(v.PlayPathAacv224), C.CString(v.PlayPathAacv164)))
	}

	cArray := C.newCArray(ptrArray, C.int(len(playlist.List)))
	cPlaylist := C.newPlaylist(C.int(playlist.MaxPageID), unsafe.Pointer(cArray))
	return C.newData(unsafe.Pointer(cPlaylist))
}

//export CgoGetVipAudioInfo
func CgoGetVipAudioInfo(trackID C.int, cookie *C.char) *C.DataError {
	ai, err := xmly.GetVipAudioInfo(int(trackID), C.GoString(cookie))
	if err != nil {
		return C.newDataError(nil, C.CString(err.Error()))
	}
	//将特殊字符替换为空格
	ai.Title = fileNameRegexp.ReplaceAllLiteralString(ai.Title, " ")
	ai.Title = strings.ReplaceAll(ai.Title, "\t", "")
	ai.Title = strings.TrimSpace(ai.Title)
	return C.newData(C.newAudioItem(C.int(ai.TrackID), C.CString(ai.Title), nil, nil, nil,
		C.CString(ai.PlayPathAacv164)))
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

	resp, err := http.Head(url)
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

	resp, err = xmly.HttpGet(url, xmly.Android)
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
		C.UpdateFileLength(uflCallback, id, &contentLength, &currentLength)
	}
	_, err = io.Copy(io.MultiWriter(file, downloadProgress), resp.Body)
	if err != nil {
		return C.CString(fmt.Sprintf("io copy %s fail: %s", filePath, err.Error()))
	}

	return nil
}

//export CgoGetUserInfo
func CgoGetUserInfo(cookie *C.char) *C.DataError {
	ui, err := xmly.GetUserInfo(C.GoString(cookie))
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

//export CgoGetQRCode
func CgoGetQRCode() *C.DataError {
	qrCode, err := xmly.GetQRCode()
	if err != nil {
		return C.newDataError(nil, C.CString(err.Error()))
	}

	p := C.newQRCode(C.int(qrCode.Ret), C.CString(qrCode.Msg), C.CString(qrCode.QrID), C.CString(qrCode.Img))
	return C.newData(p)
}

//export CgoCheckQRCode
func CgoCheckQRCode(qrID *C.char) *C.char {
	status, cookie, err := xmly.CheckQRCodeStatus(C.GoString(qrID))
	if err != nil {
		return nil
	}
	if status.Ret == 0 {
		return C.CString(cookie)
	}
	return nil
}

////export CgoGetAllAudioInfo
//func CgoGetAllAudioInfo(albumID C.int) *C.DataError {
//	list, err := xmly.GetAllAudioInfo(int(albumID))
//	if err != nil {
//		return C.newDataError(nil, C.CString(err.Error()))
//	}
//
//	ptrArray := C.newPointerArray(C.int(len(list)))
//	for i, v := range list {
//		//将特殊字符替换为空格
//		v.Title = fileNameRegexp.ReplaceAllLiteralString(v.Title, " ")
//		C.setPointerArray(ptrArray, C.int(i), C.newAudioItem(C.int(v.TrackID), C.CString(v.Title),
//			C.CString(v.PlayURL32), C.CString(v.PlayURL64), C.CString(v.PlayPathAacv224), C.CString(v.PlayPathAacv164)))
//	}
//	cArray := C.newCArray(ptrArray, C.int(len(list)))
//
//	p := C.newData(unsafe.Pointer(cArray))
//	return p
//}
