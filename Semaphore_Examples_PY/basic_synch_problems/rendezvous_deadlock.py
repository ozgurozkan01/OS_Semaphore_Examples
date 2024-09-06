import threading

"""
        DEADLOCK
        This is when threads wait for each other and never continue. In this case the program must be killed.

                Thread A            Thread B
        1 statement a1              1 statement b1
        2 bArrived.wait()           2 aArrived.wait()
        3 aArrived.signal()         3 bArrived.signal()
        4 statement a2              4 statement b2

        In this example both threads wait each other forever because of no signaling.

         - CODE OUTPUT !!
         No fully output. Because of  no signaling. Both threads are writing the first statements but not writing the second one.
         Becuase both threads call the acquire() first but not get in and call release() because of the semaphores initial value is 0
         and that causes the deadlocks.
"""

aArrived = threading.Semaphore(0)
bArrived = threading.Semaphore(0)

def threadA():
    print('statement a1')
    bArrived.acquire()
    aArrived.release()
    print('statement a2')

def threadB():
    print('statement b1')
    aArrived.acquire()
    bArrived.release()
    print('statement b2')

# Asymmetric solution : each thread calls the different function.
tA = threading.Thread(target=threadA)
tB = threading.Thread(target=threadB)

tA.start()
tB.start()

tA.join()
tB.join()