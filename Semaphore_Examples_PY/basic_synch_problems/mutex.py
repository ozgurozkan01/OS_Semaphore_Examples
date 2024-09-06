import threading

"""
        - REMINDING !!
         The mutex guarantees that only one thread accesses the shared variable at a time.
         To examine the example you can go to syncronization examples.
    
        Thread A                    Thread B
        1 count = count + 1         1 count = count + 1

        --------- * --------- * --------- * --------- * ---------          

        Thread A                    Thread B
        1 mutex . wait ()           1 mutex . wait ()
        2 # critical section        2 # critical section
        3 count = count + 1         3 count = count + 1
        4 mutex . signal ()         4 mutex . signal ()
        
        If we do not use mutex, result can return as different from 2.
"""

x = 0
mutex = threading.Lock()

def increase():
    global x
    mutex.acquire()
    temp = x
    x = temp + 1
    mutex.release()

tA = threading.Thread(target=increase)
tB = threading.Thread(target=increase)

tA.start()
tB.start()

tA.join()
tB.join()

print(f"Result : {x}")