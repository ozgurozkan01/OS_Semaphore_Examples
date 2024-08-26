import threading

"""
        Most of the time, most variables in most threads are local,
        meaning that they belong to a single thread and no other threads can access them.
        As long as that’s true, there tend to be few synchronization problems,
        because threads just don’t interact

         But usually some variables are shared among two or more threads; this is one of the ways threads interact with each other.
         For example, one way to communicate information between threads is for one thread to read a value written by another thread

         Thread A           Thread B
         x = 5              x = 7
         print x
"""

def thread_a():
    global x
    x = 5
    print(x)

def thread_b():
    x = 7

a = threading.Thread(target=thread_a,)
b = threading.Thread(target=thread_b,)

a.start()
b.start()

a.join()
b.join()

print(f"Final value of x: {x}")