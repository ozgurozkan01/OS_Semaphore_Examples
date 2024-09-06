import threading

"""
         - CODE LOGIC !!
         When n-1 thread reaches acquire(), none of them can enter, because the initial value of the barrier is already 0.
         The nth thread releases the barrier and the value of the barrier increases by 1.
         This increase allows the first thread that reaches acquire() to enter.
         In the next line, the thread calls release(), increases the barrier by 1 and
         the threads waiting at acquire() can enter respectively. Here, the value of the barrier is continuously decreasing and
         increasing by 1 0 1 0 1 0 1 0 and the thread that enters allows the next thread to enter.

         - CODE OUTPUT !!
         Output is all threads write "Barrier reached to the end!" as output.
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
    barrier.release()


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
