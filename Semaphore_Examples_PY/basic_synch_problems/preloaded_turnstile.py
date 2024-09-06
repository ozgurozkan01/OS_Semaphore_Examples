import threading

"""
    - DEFINITION OF PRELOADED TURNSTILE !!
    A preloaded turnstile is a turnstile synchronization component that is initialized with
    a predefined number of signals before threads attempt to pass through it.
    This preloading mechanism allows a specified number of threads to pass through the turnstile simultaneously,
    reducing the overhead associated with sequential thread passage.

    - LOGIC OF RUNNING !!
    When the nth thread arrives, it preloads the first turnstile with one signal for each thread.
    When the nth thread passes the turnstile, it “takes the last token” and leaves the turnstile locked again.
    The same thing happens at the second turnstile, which is unlocked when the last thread goes through the mutex.
    
    - CODE OUTPUT !!
    The output is n pieces “Preloaded barrier reached to the end!”.
"""


n = 5
count = 0
mutex = threading.Semaphore(1)
turnstile = threading.Semaphore(0)
turnstile2 = threading.Semaphore(0)

def execute():
    global count

    mutex.acquire() # for synch
    count += 1
    if count == n: turnstile.release(n) # Unlock the tunrstile for first thread
    mutex.release()

    turnstile.acquire() # Blocking for threads

    mutex.acquire() # for synch
    count -= 1
    if count == 0: turnstile2.release(n) # Unlock the tunrstile2 for first thread
    mutex.release()

    turnstile2.acquire() # Blocking for threads


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