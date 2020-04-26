package tool

import (
    "math"
    "math/rand"
    "time"
	"bytes"
	"runtime"
    // "fmt"
)

type Tool struct {}

func (this Tool) Rand(st int, ed int) int {
	x := 0
	rand.Seed(time.Now().UnixNano())
	for i := 0; ed > 0; ed /= 10 {
		s := st % 10
		e := ed % 10
		rd := rand.Intn(e)
		x += (rd % (e - s) + s) * int(math.Pow10(i))
		i++
		st /= 10
	}
	return x
}

func (this Tool) RandN(n int) int {
	x := 0
	rand.Seed(time.Now().UnixNano())
	for i := 0; i < n; i++ {
		rd := rand.Intn(10)
		x += rd * int(math.Pow10(i))
	}
	return x
}

func (this Tool) RandNStr(n int) string {
	x := 0
	rand.Seed(time.Now().UnixNano())
	for i := 0; i < n; i++ {
		rd := rand.Intn(10)
		x += rd * int(math.Pow10(i))
	}
	return fmt.Sprintf("%0"+strconv.Itoa(n)+"d", x)
}

func (this Tool) GetGID() string {
       b := make([]byte, 64)
       b = b[:runtime.Stack(b, false)]
       b = bytes.TrimPrefix(b, []byte("goroutine "))
       b = b[:bytes.IndexByte(b, ' ')]
       return string(b)
}

func (this Tool) Ksort(params map[string]string) map[int][]string {
	i := 0
	r := make(map[int][]string)
	for k,v := range params {
		r[i] = []string{k,v}
		for j := i-1; j >= 0; j-- {
			if k < r[j][0] {
				r[j],r[j+1] = r[j+1],r[j]
			} else {
				break;
			}
		}
	}
	return r
}