import threading

'''
         - WHY MORRIS'S ALGORITHM IS USED !!
            Starvation can occur when one or more threads are perpetually denied
            access to a resource because other threads continually consume the resource.

            Morris’s algorithm helps prevent starvation by ensuring that all threads will eventually gain
            access to the critical section, even if there are more threads trying to enter concurrently

          - WHY THERE ARE 2 PHASES !!
            Phase 1 ensures that threads are managed fairly before they enter the critical section.
            By using t1 and t2, the algorithm controls access to the critical section,
            ensuring only one thread executes the critical section at a time.
'''

mutex = threading.Semaphore(1)
room1 = 0
room2 = 0
t1 = threading.Semaphore(1)
t2 = threading.Semaphore(0)

def morris_algorithm():
    global room1
    global room2

    while True:
        mutex.acquire()
        room1 += 1
        mutex.release()

        t1.acquire()
        room2 += 1
        mutex.acquire()
        room1 -= 1

        if room1 == 0:
            mutex.release()
            t2.release()
        else:
            mutex.release()
            t1.release()

        t2.acquire()
        room2 -= 1

        print('Critical Section\n')
        if room2 == 0:  t1.release()
        else:           t2.release()


thread1 = threading.Thread(target=morris_algorithm)
thread2 = threading.Thread(target=morris_algorithm)
thread3 = threading.Thread(target=morris_algorithm)

thread1.start()
thread2.start()
thread3.start()

thread1.join()
thread2.join()
thread3.join()
