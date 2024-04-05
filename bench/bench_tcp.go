package main

import (
	"flag"
	"fmt"
	"log"
	"net"
	"os"
	"sync"
	"sync/atomic"
	"time"
)

var (
	c = flag.Int("c", 5, "concurrency")
	t = flag.Int("t", 10, "duration in seconds")
)

func main() {
	flag.Parse()

	if len(os.Args) < 3 {
		fmt.Printf("Usage %s [address]\n", os.Args[0])
		os.Exit(1)
	}

	wg := sync.WaitGroup{}
	msg := []byte("hello")
	nout := uint64(0)
	nin := uint64(0)
	timer := time.NewTimer(time.Duration(*t) * time.Second)
	defer timer.Stop()

	fmt.Printf("Sending requests...\n")

	for i := 0; i < *c; i++ {
		wg.Add(1)
		go func(j int) {
			addr := os.Args[1]
			conn, err := net.Dial("tcp", addr)
			if err != nil {
				log.Fatalf("net.Dial failed with %v", err)
			}
			conn.SetDeadline(time.Now().Add(time.Millisecond * 5))
			// fmt.Printf("Connected %d\n", j)
			buf := make([]byte, len(msg))
			for {
				select {
				case <-timer.C:
					goto done
				default:
				}
				if _, err = conn.Write(msg); err != nil {
					log.Printf("conn.Write failed with %v", err)
					break
				}
				atomic.AddUint64(&nout, 1)
				if _, err = conn.Read(buf); err != nil {
					log.Printf("conn.Read failed with %v", err)
					break
				}
				atomic.AddUint64(&nin, 1)
			}
		done:
			wg.Done()
		}(i)
	}

	wg.Wait()

	fmt.Printf("Speed:    %d reqs/sec\n", nout/uint64(*t))
	fmt.Printf("Sent:     %d\n", nout)
	fmt.Printf("Received: %d\n", nin)
}
