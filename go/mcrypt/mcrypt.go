package mcrypt

import (
	"crypto/md5"
	"encoding/hex"
	"bytes"
    "crypto/aes"
    "crypto/cipher"
    "encoding/base64"
    // "fmt"
)

// const KEY string = "9871267812345mn812345xyz"
const KEY string = "qh360hg43df5hdf3g4ds3fg4"

func Md5(str string) string {
	h := md5.New()
	h.Write([]byte(str))
	return hex.EncodeToString(h.Sum(nil))
}

func StrRot13(s string) string {
	return str_rot(s, 13)
}

func str_rot(s string, n int) string {
	n = n % 26
	if n == 0 {
		return s
	}
	l := len(s)
	sb := make([]byte, len(s))
	for i := 0; i < l; i++ {
		// fmt.Printf("%#v %c ", s[i])
		c := int(s[i])
		if c >= 'a' && c <= 'z' {
			sb[i] = byte((c - 71 + n) % 26 + 97)
		} else if c >= 'A' && c <= 'Z' {
			sb[i] = byte((c - 39 + n) % 26 + 65)
		} else {
			sb[i] = s[i]
		}
	}
	// fmt.Printf("%s\n", string(sb))
	return string(sb)
}

func AesEn(orig string) string {
    // 转成字节数组
    origData := []byte(orig)
    k := []byte(KEY)
    // 分组秘钥
    block, _ := aes.NewCipher(k)
    // 获取秘钥块的长度
    blockSize := block.BlockSize()
    // 补全码
    origData = PKCS7Padding(origData, blockSize)
    // 加密模式
    blockMode := cipher.NewCBCEncrypter(block, k[:blockSize])
    // 创建数组
    cryted := make([]byte, len(origData))
    // 加密
    blockMode.CryptBlocks(cryted, origData)
    return base64.StdEncoding.EncodeToString(cryted)
}
func AesDe(cryted string) string {
    // 转成字节数组
    crytedByte, _ := base64.StdEncoding.DecodeString(cryted)
    k := []byte(KEY)
    // 分组秘钥
    block, _ := aes.NewCipher(k)
    // 获取秘钥块的长度
    blockSize := block.BlockSize()
    // 加密模式
    blockMode := cipher.NewCBCDecrypter(block, k[:blockSize])
    // 创建数组
    orig := make([]byte, len(crytedByte))
    // 解密
    blockMode.CryptBlocks(orig, crytedByte)
    // 去补全码
    orig = PKCS7UnPadding(orig)
    return string(orig)
}
//补码
func PKCS7Padding(ciphertext []byte, blocksize int) []byte {
    padding := blocksize - len(ciphertext)%blocksize
    padtext := bytes.Repeat([]byte{byte(padding)}, padding)
    return append(ciphertext, padtext...)
}
//去码
func PKCS7UnPadding(origData []byte) []byte {
    length := len(origData)
    unpadding := int(origData[length-1])
    return origData[:(length - unpadding)]
}