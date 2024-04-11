package main

import (
	"flag"
	"fmt"
	"net"
	"os"
	"sync"
	"sync/atomic"
	"time"
)

var (
	c    = flag.Int("c", 5, "concurrency")
	t    = flag.Int("t", 10, "duration in seconds")
	unix = flag.Bool("unix", false, "use UNIX domain socket")
)

func main() {
	flag.Parse()

	if len(os.Args) < 2 {
		fmt.Printf("Usage %s [address]\n", os.Args[0])
		os.Exit(1)
	}

	wg := sync.WaitGroup{}
	mode := "tcp"
	msg := []byte("hello")
	nout := uint64(0)
	nin := uint64(0)
	timer := time.NewTimer(time.Duration(*t) * time.Second)
	defer timer.Stop()

	fmt.Printf("Sending requests...\n")
	now := time.Now()

	for i := 0; i < *c; i++ {
		wg.Add(1)
		go func(j int) {
			defer wg.Done()

			addr := flag.Args()[0]
			if *unix {
				mode = "unix"
			}
			conn, err := net.Dial(mode, addr)
			if err != nil {
				fmt.Printf("Connecting %d: %v\n", j, err)
				return
			}
			conn.SetDeadline(time.Now().Add(time.Second))
			buf := make([]byte, len(msg))
			turns := 0
			for {
				select {
				case <-timer.C:
					return
				default:
				}
				if _, err = conn.Write(msg); err != nil {
					fmt.Printf("Connection %d Turn %d: %v\n", j, turns, err)
					break
				}
				atomic.AddUint64(&nout, 1)
				if _, err = conn.Read(buf); err != nil {
					fmt.Printf("Connection %d Turn %d: %v\n", j, turns, err)
					break
				}
				atomic.AddUint64(&nin, 1)
				turns++
			}
		}(i)
	}

	wg.Wait()

	d := time.Since(now)
	fmt.Printf("Duration: %s\n", d)
	fmt.Printf("Sent:     %d\n", nout)
	fmt.Printf("Received: %d\n", nin)
	fmt.Printf("Speed:    %d reqs/sec\n", uint64(time.Second)*nout/uint64(d))
}
