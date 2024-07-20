import threading

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
