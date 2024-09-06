import threading

"""
        - WHY IT IS NOT WORKING !!
        Since the initial value of the barrier semaphore is set to 0, any thread cannot acquire this semaphore 
        when it reaches barrier.acquire(). This means that the semaphore is “closed” and the first thread (thread1) 
        will start waiting when it reaches barrier.acquire(). This waiting process means that the thread cannot pass this point.

        As a result, thread1 is stuck at this point and does not release the mutex semaphore it received earlier. 
        In this case, all other threads also start waiting to access the mutex. However, since the mutex is never released,
         the other threads cannot enter the critical region and wait forever.

        Because of this deadlock, there is no output from the program and all threads continue to wait.
"""

n = 5
count = 0
mutex = threading.Semaphore(1)
barrier = threading.Semaphore(0)

def execute():
    global count
    mutex.acquire()
    count += 1

    if count == n: barrier.release()

    barrier.acquire()
    barrier.release()

    mutex.release()

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
