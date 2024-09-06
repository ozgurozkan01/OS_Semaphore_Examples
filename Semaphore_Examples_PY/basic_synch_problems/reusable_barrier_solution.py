import threading

"""
         To solve deadlock problem, we use 2 turnstile semaphore.

         turnstile:
         This semaphore is used to synchronize threads at the first barrier point.
         It ensures that all threads wait until the last thread has reached the barrier before proceeding.
         It is initialized to 0, meaning threads will block on turnstile.acquire() until it is released by the last thread.

         turnstile2:
         This semaphore is used to manage a second phase of synchronization, essentially acting as a signal for the second barrier point.
         It is initialized to 1, so it starts in the "unlocked" state. This allows the first thread to proceed after all threads have passed the first barrier.
         It ensures that once all threads have reached the first barrier and proceeded, they wait for all threads to reach the second barrier before continuing.

         - CODE OUTPUT !!
         The output is n pieces “Reusable barrier reached to the end!”.
"""

import threading

n = 5
count = 0
mutex = threading.Lock()
turnstile = threading.Semaphore(0)
turnstile2 = threading.Semaphore(1)

def execute():
    global count

    # First section: Using lock for synchronization
    with mutex:
        count += 1
        if count == n:
            turnstile2.acquire()  # Lock turnstile2
            turnstile.release()   # Unlock turnstile for the first thread

    turnstile.acquire()  # Blocking for the first thread
    turnstile.release()  # When the first thread is released, it releases the thread that comes after it

    # Second section: Using lock for synchronization again
    with mutex:
        count -= 1
        if count == 0:
            turnstile.acquire()  # Lock turnstile
            turnstile2.release() # Unlock turnstile2 for the first thread

    turnstile2.acquire()  # Blocking for the first thread
    turnstile2.release()  # When the first thread is released, it releases the thread that comes after it

    print("Reusable barrier reached to the end!")


threads = []
for _ in range(n):
    t = threading.Thread(target=execute)
    threads.append(t)
    t.start()

for t in threads:
    if t.is_alive():
        t.join()