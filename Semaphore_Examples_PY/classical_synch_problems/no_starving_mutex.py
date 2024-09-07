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

                Phase 1: Each thread first increments the room1 counter and then waits for the t1 semaphore.
                This allows the thread to synchronize with other threads.
                If room1 is zero, this indicates that the thread is the first thread, so the t2 semaphore is released.
                The other threads release the t1 semaphore and go back to waiting.

                Phase 2: After waiting for semaphore t2, the thread accesses the critical region.
                After the critical zone operation, the thread decrements the room2 counter and if room2 is zero,
                it allows other threads to wait for the t1 semaphore.
                Otherwise, the t2 semaphore is released and other threads access the critical region.

            - CODE OUTPUT !!
            1. Phase - Room Management and Access Control:
                The first phase of the code ensures that each thread gets a fair turn before entering the
                critical area. In this phase, a thread indicates that it is in the waiting room by incrementing
                the counter named “room1” by one. Then, the thread waits to obtain the t1 semaphore.
                The semaphore determines the first thread that can enter the critical region.

                If room1 is zero (i.e. there are no other waiting threads), this thread releases semaphore t2
                and can enter the critical region. Other threads release semaphore t1 and wait for their
                turn to enter the critical region.

            2. Phase - Critical Zone Access and Exit:
                In the second phase, the thread waits for the t2 semaphore before entering the critical
                region. After accessing the critical region, the thread decrements the “room2” counter
                value by one and executes its operations in the critical region.

                When exiting the critical zone, if room2 has reached zero (i.e. there are no other threads
                waiting), it releases semaphore t1 and allows other threads to wait in phase one.
                Otherwise, it releases the t2 semaphore, allowing other threads to access the critical
                region.
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
