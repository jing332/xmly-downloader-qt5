package main

import (
	"log"
	xmldown "ximalaya-spider/xmlydownloader"
)

func main() {
	//Cookie from https://www.52pojie.cn/thread-1201247-1-1.html
	cookie := "1&_token=180933016&DA5D3ECDB9B8421887875173711A02F5NdV204BEB442C4A98BE39BD8E595FBA3AC9BDADCA29B10C458765683DD8EBD5541A"
	albumId := 23457286
	err, _ := xmldown.GetAudioListByPageId(albumId, 1)
	if err != nil {
		log.Fatal(err)
	}

	ai, err := xmldown.GetVipAudioInfo(247826907, cookie)
	if err != nil {
		log.Fatal(err)
	}
	log.Println(ai)
}
