import threading

"""
         This code seems to write corrcet but there is some kind of synch problems.

         Potential Deadlock:
         The second turnstile.acquire() could cause a deadlock because if the last thread to decrement count
         is the same one that released the semaphore, no other thread will be able to release it again.

         - CODE OUTPUT !!
         The code can give output and this output is “Reusable barrier reached to the end!” as many as the number of threads.
         However, due to a logic error in the code, the code may enter deadlock. In this case there is no output.
"""

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