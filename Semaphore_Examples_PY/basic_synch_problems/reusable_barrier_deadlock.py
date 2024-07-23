import threading

n = 3 # amount of threads
count = 0 # counted thread amount
mutex = threading.Semaphore(1)
turnstile = threading.Semaphore(0)

# This code has deadlock problem.
# Because Python’s GIL can lead to different scheduling and context-switching behavior compared to C++.
# This can affect the timing of semaphore operations and thread execution, making deadlocks more likely.

# The final turnstile.acquire() call when count reaches 0.
# This acquire operation happens when no threads are left to release the turnstile,
# resulting in the thread being blocked indefinitely.

def thread_run():
    global count

    mutex.acquire()
    count += 1
    mutex.release()

    if count == n: turnstile.release()

    turnstile.acquire()
    turnstile.release()

    mutex.acquire()
    count -= 1
    mutex.release()

    if count == 0: turnstile.acquire()

t1= threading.Thread(target=thread_run)
t2 = threading.Thread(target=thread_run)
t3 = threading.Thread(target=thread_run)

t1.start()
t2.start()
t3.start()

t1.join()
t2.join()
t3.join()