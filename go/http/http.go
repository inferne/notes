package http
	
import (
	"io/ioutil"
	"net/http"
	"net"
	"time"
	"bytes"
	// "strings"
	"mime/multipart"
	"fmt"
)

func transport() (tr *http.Transport) {
	return &http.Transport{
		Dial: func(netw, addr string) (net.Conn, error) {
            conn, err := net.DialTimeout(netw, addr, time.Second * 3)    //设置建立连接超时
            if err != nil {
                return nil, err
            }
            conn.SetDeadline(time.Now().Add(time.Second * 3))    //设置发送接受数据超时
            return conn, nil
        },
        ResponseHeaderTimeout: time.Second * 3,
	}
}

func Get(url string) ([]byte, error) {
	client := &http.Client{Transport: transport()}
	
	req, err := http.NewRequest("GET", url, nil)
	if err != nil {
		fmt.Println("NewRequest", url, err)
		return nil, err
	}
	req.Header.Set("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3")
	req.Header.Set("User-Agent", "Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/76.0.3809.132 Safari/537.36")
	resp, err := client.Do(req)
	// fmt.Printf("%+v\n", resp)
	if err != nil || resp.StatusCode != 200 {
		fmt.Println("Do", url, err)
		return nil, err
	}
	body, err := ioutil.ReadAll(resp.Body)
	if err != nil {
		fmt.Println(err)
	}
	
	defer resp.Body.Close()
	return body, err
}

func PostForm(url string, data map[string]string) ([]byte, error) {
	client := &http.Client{Transport: transport()}
	// fmt.Printf("%+v\n", FormData(data))
	bd := &bytes.Buffer{}
	writer := multipart.NewWriter(bd)
	for k,v := range data {
		_ = writer.WriteField(k, v)
	}
	writer.Close()
	// req, err := http.NewRequest("POST", url, strings.NewReader(BuildQuery(data)))
	req, err := http.NewRequest("POST", url, bd)
	if err != nil {
		fmt.Println("NewRequest", url, err)
		return []byte{}, err
	}
    req.Header.Set("Content-Type", writer.FormDataContentType())

    resp, err := client.Do(req)
 
    body, err := ioutil.ReadAll(resp.Body)
    if err != nil {
		fmt.Println(err)
    }
 
    defer resp.Body.Close()
    return body, err
}

func BuildQuery(data map[string]string) string {
	query := ""
	for k,v := range data {
		if len(query) > 0 {
			query += "&"
		}
		query += k + "=" + v

	}
	// fmt.Printf("%+v\n", query)
	return query
}

var h = []string{"0","1","2","3","4","5","6","7","8","9","A","B","C","D","E","F"}

func UrlEncode(bytes []byte) string {
	str := ""
	for _,v := range bytes {
		str += "%" + fmt.Sprintf("%02s", conv10to16(int(v), 16))
	}
	return str
}

var reserved = map[byte]string{'!':"%21", '#':"%23", '$':"%24", '&':"%26", '\'':"%27", '(':"%28", ')':"%29", '*':"%2A", '+':"%2B", ',':"%2C", '/':"%2F", ':':"%3A", ';':"%3B", '=':"%3D", '?':"%3F", '@':"%40", '[':"%5B", ']':"%5D"}

func UrlEncodeReserved(bytes []byte) string {
	str := ""
	for _,v := range bytes {
		if _,ok := reserved[v]; ok {
			str += reserved[v]
		} else {
			str += string(v)
		}
	}
	return str
}

func conv10to16(n, num int) string {
	result := ""
	for ; n > 0; n /= num {
		lsb := h[n%num]
		result = lsb + result
	}
	return result
}
