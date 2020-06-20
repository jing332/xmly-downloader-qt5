package xmlydecrypt

import (
	"fmt"
	"github.com/robertkrimen/otto"
	"log"
	"strconv"
	"strings"
	"sync"
)

var (
	jsVM  = otto.New()
	mutex sync.Mutex
)

func init() {
	_, err := jsVM.Run(XmlyDecryptSrc)
	if err != nil {
		log.Printf("JavaScript parse fail: %v", err)
	}
}

func DecodeFileParams(ep string) (err error, sign string, buyKey, token, timestamp int64) {
	mutex.Lock()
	v, err := jsVM.Call("_t", nil, ep)
	mutex.Unlock()
	if err != nil {
		return fmt.Errorf("JavaScript function call fail: %v", err), "", -1, -1, -1
	}

	obj := v.Object()
	jsSign, err := obj.Get("sign")
	jsBuyKey, err := obj.Get("buy_key")
	jsToken, err := obj.Get("token")
	jsTimestamp, err := obj.Get("timestamp")
	if err != nil {
		return fmt.Errorf("JavaScript return value handle fail: %v", err), "", -1, -1, -1
	}

	buyKey, err = jsBuyKey.ToInteger()
	if err != nil {
		return fmt.Errorf("JavaScript buykey.ToInteger fail: %v", err), "", -1, -1, -1
	}

	token, err = jsToken.ToInteger()
	if err != nil {
		return fmt.Errorf("JavaScript token.ToInteger fail: %v", err), "", -1, -1, -1
	}

	timestamp, err = jsTimestamp.ToInteger()
	if err != nil {
		return fmt.Errorf("JavaScript timestamp.ToInteger fail: %v", err), "", -1, -1, -1
	}

	return nil, jsSign.String(), buyKey, token, timestamp
}

func DecodeFileName(send int, s string) string {
	x := New(float64(send))

	uri := x.CgFun(s)
	if "/" != string(uri[0]) {
		uri = "/" + uri
	}
	return uri
}

type Xmly struct {
	CgStr      string
	RandomSeed float64
}

func New(send float64) *Xmly {
	x := &Xmly{}
	x.RandomSeed = send
	x.CgHun()
	return x
}

func (x *Xmly) Ran() float64 {
	j := 211*x.RandomSeed + 30031
	x.RandomSeed = float64(int(j) % 65536)
	return x.RandomSeed / float64(65536)
}

func (x *Xmly) CgHun() {
	x.CgStr = ""
	key :=
		"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ/\\:._-1234567890"

	kLen := len(key)
	for i := 0; i < kLen; i++ {
		ran := x.Ran()
		r := int(ran * float64(len(key)))

		x.CgStr += string([]byte(key)[r])
		key = strings.ReplaceAll(key, string([]byte(key)[r]), "")
	}
}

func (x *Xmly) CgFun(t string) string {
	strs := strings.Split(t, "*")

	e := ""
	for n := 0; n < len(strs)-1; n++ {
		if strs[n] != "" {
			index, err := strconv.Atoi(strs[n])
			if err != nil {
				log.Fatal(err)
			}
			e += string([]byte(x.CgStr)[index])
		}
	}
	return e
}

const O = "g3utf1k6yxdwi0"

var U = []int{
	19,
	1,
	4,
	7,
	30,
	14,
	28,
	8,
	24,
	17,
	6,
	35,
	34,
	16,
	9,
	10,
	13,
	22,
	32,
	29,
	31,
	21,
	18,
	3,
	2,
	23,
	25,
	27,
	11,
	20,
	5,
	15,
	12,
	0,
	33,
	26,
}


//func charCodeAt(s string, n int) rune {
//	i := 0
//	for _, r := range s {
//		if i == n {
//			return r
//		}
//		i++
//	}
//	return 0
//}

//func GenKey(e, t string) string {
//	var (
//		n = 0
//		r = []int{}
//		a = 0
//		i = ""
//		o = 0
//	)
//
//	for o = 0; o < 256; o++ {
//		r = append(r, o)
//	}
//
//	for o = 0; o < 256; o++ {
//		a = a + r[o] + int(charCodeAt(e, o%len(e)))%256
//		n = r[o]
//		r[o] = r[a]
//		r[a] = n
//	}
//
//
//
//	for
//
//
//
//}


//
//func GenKeyE(e string, t []int) string {
//	var n []string
//	for r := 0; r < len(e); r++ {
//		a := []rune("a")[0]
//		if int16(a) <= int16(e[r]) && int16([]rune("z")[0]) >= int16(e[r]) {
//			a = rune(int16(charCodeAt(e, r)) - 97)
//		} else {
//			a = rune(int16(charCodeAt(e, r)) - 48 + 26)
//		}
//
//		n = append(n, "")
//		for i := 0; 36 > i; i++ {
//			if int16(t[i]) == int16(a) {
//				a = rune(i)
//				break
//			}
//		}
//
//		if 25 < a {
//			n[r] = string(int16(a) - 26 + 48)
//		} else {
//			n[r] = string(int16(a) + 97)
//		}
//	}
//
//	var str string
//	if len(n) != 0 {
//		for _, s := range n {
//			str += s
//		}
//	} else {
//		str = ""
//	}
//
//	return str
//}
