import threading

"""
    Global Interpreter Lock (GIL): Python uses a GIL which ensures that only one thread
    executes Python bytecode at a time. This serialization inherently prevents multiple
    threads from concurrently accessing or modifying Python objects like integers (count
    in our case).
    
    In this code there is actually a sync problem, But GIL in Python prevents this.

    At first glance, it is not obvious that there is a synchronization problem here.
    There are only two execution paths, and they yield the same result.
    The problem is that these operations are translated into machine language before execution,
    and in machine language the update takes two steps, a read and a write.

    Signaling makes it possible to guarantee that a section of code in one thread
    will run before a section of code in another thread; in other words, it solves the
    serialization problem.

        Thread A               Thread B
        count = count + 1      1 count = count + 1

    So the problem is that here each thread is reading the same value. Because of that,
    the value is not going increased 1 instead of 2.
    
    In this problem, unless C++, code works with bigger values such as 1.000.000 . 
    So, the output returns as 2.000.000 . But it is unpredictable.
"""

counter = 0

def increment():
    global counter
    counter += 1

thread1 = threading.Thread(target=increment)
thread2 = threading.Thread(target=increment)

thread1.start()
thread2.start()

thread1.join()
thread2.join()

print(counter)
# This can print either 1 or 2