import threading

"""
        - BARRIER DEFINITION !!
       A barrier is a synchronization mechanism that enforces a condition where a specified number of threads
       must all reach a particular point of execution (the "barrier") before any of them are allowed to proceed beyond that point.
       This ensures that all threads are synchronized at the barrier, making it useful for tasks that require collective agreement or coordination.

       - PROGRAM DEFINITON !!
       Let's consider the Rendezvous problem again. In this problem, we could not use more than 2 threads.
       To provide the synchronization no thread should execute critical point until after all threads have executed rendezvous.
       TO provide this, we are gonna use barrier. Barrier is gonna be locked (0 or negative) until all thread arrives to the barrier.
       When this condition is true, barrier will be unlocked (1 or more).

       1 n = the number of threads
       2 count = 0                 --> keeps track of how many threads have arrived
       3 mutex = Semaphore (1)
       4 barrier = Semaphore (0)

       WHY SOLUTION IS NOT CORRECT ?
       Specifically, all threads call barrier.acquire() after incrementing the count,
       but only the last thread (when count == n) calls barrier.release().
       This causes the first n-1 thread to be blocked indefinitely in barrier.acquire()
       because barrier.release() is called only once or for one thread.

       To solve this problem, we need to make sure that all threads can continue after barrier.
       Solution I found is that when n == count, then barrier should be released for all threads.
"""

n = 5
count = 0
mutex = threading.Semaphore(1)
barrier = threading.Semaphore(0)

def execute():
    global count
    mutex.acquire()
    count += 1
    mutex.release()

    if count == n: barrier.release()

    barrier.acquire()

t1 = threading.Thread(target=execute)
t2 = threading.Thread(target=execute)
t3 = threading.Thread(target=execute)
t4 = threading.Thread(target=execute)
t5 = threading.Thread(target=execute)

t1.start()
t2.start()
t3.start()
t4.start()
t5.start()

t1.join()
t2.join()
t3.join()
t4.join()
t5.join()
