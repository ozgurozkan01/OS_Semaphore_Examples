import threading

"""
         In this implementation, the output should be the :
                statement a1
                statement b1
         Because to be able to call the acquire() semaphore should have a positive value.
         But initialized with 0. Because of that, the acquire cannot be called.
         First semaphore should be released. So, the first statement is going to be the a1.
         After releasing, b1 can be written as an output. Until releasing, threadB will wait.
"""

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