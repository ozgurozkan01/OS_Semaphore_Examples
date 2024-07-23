import threading

# This code seems correct, but it has some synch problems which can cause deadlock.
# 

n = 3 # amount of threads
count = 0 # counted thread amount
mutex = threading.Semaphore(1)
turnstile = threading.Semaphore(0)

def thread_run():
    global count

    mutex.acquire()
    count += 1
    if count == n: turnstile.release()
    mutex.release()

    turnstile.acquire()
    turnstile.release()

    mutex.acquire()
    count -= 1
    if count == 0: turnstile.acquire()
    mutex.release()

t1= threading.Thread(target=thread_run)
t2 = threading.Thread(target=thread_run)
t3 = threading.Thread(target=thread_run)

t1.start()
t2.start()
t3.start()

t1.join()
t2.join()
t3.join()