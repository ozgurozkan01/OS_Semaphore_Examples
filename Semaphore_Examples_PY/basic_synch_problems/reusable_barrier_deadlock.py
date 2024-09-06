import threading

n = 3 # amount of threads
count = 0 # counted thread amount
mutex = threading.Semaphore(1)
turnstile = threading.Semaphore(0)

"""
        Code seems to work, but unfortunately there are logic errors inside that can cause a synchronization error.
        This code has deadlock problem.
        
        Because Python’s GIL can lead to different scheduling and context-switching behavior compared to C++.
        This can affect the timing of semaphore operations and thread execution, making deadlocks more likely.

        Failure Scenario !!
           First Iteration: In the first iteration, threads might reach the barrier, increment count, and correctly proceed through the semaphore.
           Subsequent Iterations: In the next iteration, the state of the semaphore may not be reset correctly:
               If a thread acquires the semaphore after the turnstile.acquire() call, but before all threads have reached the barrier in the next iteration, it will proceed incorrectly.
               Other threads will be left waiting indefinitely because the semaphore count may not align with the required number of threads.


         - CODE OUTPUT !!
         The code can give output and this output is “Reusable barrier reached to the end!” as many as the number of threads.
         However, due to a logic error in the code, the code may enter deadlock. In this case there is no output.
"""


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