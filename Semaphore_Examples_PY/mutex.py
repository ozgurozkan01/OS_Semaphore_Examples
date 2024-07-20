import threading

x = 0
mutex = threading.Lock()

def increase():
    global x
    for i in range(100):
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