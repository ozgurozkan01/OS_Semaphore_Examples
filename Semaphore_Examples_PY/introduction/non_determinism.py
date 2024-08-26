import threading

"""
         Concurrent Programs : Multiple threads of execution, all occuring at the same time.
         Concurrent programs are often non-deterministic. So, it is hard to see what will happen.

        Thread A            Thread B
        print " yes "       print " no "

         - CODE OUTPUT !!
         The order in which messages are printed depends on the running order of the threads
         because threads run in parallel with each other and there is no specific order.
         The operating system determines this order. Therefore, the order of the “YES!” and “NO!” messages may be different in each run.
"""

def thread_a():
    print('YES')

def thread_b():
    print('NO')

a = threading.Thread(target=thread_a,)
b = threading.Thread(target=thread_b,)

a.start()
b.start()

a.join()
b.join()