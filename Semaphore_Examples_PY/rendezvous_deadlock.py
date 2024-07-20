import threading

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

tA = threading.Thread(target=threadA)
tB = threading.Thread(target=threadB)

tA.start()
tB.start()

tA.join()
tB.join()