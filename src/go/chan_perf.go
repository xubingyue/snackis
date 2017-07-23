package main

import (
	"runtime"
	"sync"
)

func run_pub(wg *sync.WaitGroup, ch chan int, reps int) {
	runtime.LockOSThread()
	
	for i := 0; i < reps; i++ {
		ch<- i
	}
	
	wg.Done()
}

func run_con(wg *sync.WaitGroup, ch chan int, reps int) {
	runtime.LockOSThread()

	for i := 0; i < reps; i++ {
		<-ch
	}
	
	wg.Done()
}

func run(workers, reps, buf int) {
	var wg sync.WaitGroup
	ch := make(chan int, buf)
	
	for i := 0; i < workers; i++ {
		wg.Add(2)
		go run_pub(&wg, ch, reps)
		go run_con(&wg, ch, reps)
	}

	wg.Wait()
}

const (
	MAX_WORKERS =    5
	MAX_REPS = 1000000
	MAX_BUF =     1000
)

func main() {
	for workers := 1; workers < MAX_WORKERS; workers++ {
		for reps := 10; reps < MAX_REPS; reps *= 10 {
			for buf := 1; buf < MAX_BUF; buf *= 10 {
				run(workers, reps, buf)
			}
		}
	}
}
