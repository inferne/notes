package ipconv

import (
    "net"
    "strings"
    "strconv"
)

func UInt32ToIP(intIP uint32) net.IP {
    var bytes []byte
    bytes[0] = byte(intIP & 0xff)
    bytes[1] = byte((intIP >> 8) & 0xff)
    bytes[2] = byte((intIP >> 16) & 0xff)
    bytes[3] = byte((intIP >> 24) & 0xff)
    return net.IPv4(bytes[3], bytes[2], bytes[1], bytes[0])
}

func IPToUInt32(ipnr net.IP) uint32 {
    bits := strings.Split(ipnr.String(), ".")

    b0, _ := strconv.Atoi(bits[0])
    b1, _ := strconv.Atoi(bits[1])
    b2, _ := strconv.Atoi(bits[2])
    b3, _ := strconv.Atoi(bits[3])

    var sum uint32
    sum += uint32(b0) << 24
    sum += uint32(b1) << 16
    sum += uint32(b2) << 8
    sum += uint32(b3)
    return sum
}