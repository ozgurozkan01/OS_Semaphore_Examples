import threading

"""
        Thread A            Thread B
        1 statement a1      1 statement b1
        2 statement a2      2 statement b2

        - CODE OUTPUT !!
        In this implementation both a1 and b1 will write as output before a2 and b2.
        We have to guaratee that. And it does not matter whether a1 or b1 is written first.

        - HINT IN BOOK TO IMPLEMENTATION !!

        The chances are good that you were able to figure out a solution, but if not, here is a hint.
        Create two semaphores, named "aArrived" and "bArrived", and initialize them both to zero.
        As the names suggest, aArrived indicates whether Thread A has arrived at the rendezvous, and bArrived likewise.

        REMINDING !!!
        If current value of semaphore is 0, so thread cannot keep going. Value should be positive to be able to
        be used by thread.
        
        -- LOGIC --
        First of all, all semaphores are not available initially.
        We got that semaphores have a value of 0.
"""

aArrived = threading.Semaphore(0)
bArrived = threading.Semaphore(0)

def threadA():
    print('statement a1')
    # semaphore value is increased. Now value is 1 and it means available.
    aArrived.release()
    # semaphore value is decreased. But initially value is 0. So decreasing not valid.
    #  Because value cannot be negative. So if bArrived is not released by threadB, threadA cannot move.
    bArrived.acquire()
    print('statement a2')

def threadB():
    print('statement b1')
    # semaphore value is increased. Now value is 1 and it means available.
    bArrived.release()
    # semaphore value is decreased. But initially value is 0. So decreasing not valid.
    #  Because value cannot be negative. So if bArrived is not released by threadA, threadB cannot move.
    aArrived.acquire()
    print('statement b2')

tA = threading.Thread(target=threadA)
tB = threading.Thread(target=threadB)

tA.start()
tB.start()

tA.join()
tB.join()