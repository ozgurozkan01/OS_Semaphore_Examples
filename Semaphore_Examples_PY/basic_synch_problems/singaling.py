import threading

binarySem = threading.Semaphore(0)

def thread_a():
    print("statement a1")
    binarySem.release()

def thread_b():
    binarySem.acquire()
    print("statement b1")


t_a = threading.Thread(target=thread_a)
t_b = threading.Thread(target=thread_b)

t_b.start()  # threadB will wait on binarySem.acquire()
t_a.start()  # threadA will print statement a1 and release binarySem

t_a.join()
t_b.join()