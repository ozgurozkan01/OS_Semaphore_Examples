"""
        MULTIPLEX : To allow multiple threads run in the critical section. To do that, we can use Semaphore which is value bigger than 1.

        1 multiplex . wait ()
        2 critical section
        3 multiplex . signal ()

        In program, there are 5 threads but the semaphore has 4 as max value.
        When you run the program, 4 threads can run at the same time but
        the last one cannot get inside the semaphore. Because there is no room for it.

        We could not release the threads to show when semaphore is full threads cannot get inside.
        But at the same time, we saw that multiple threads can run at the same time

        - CODE OUTPUT !!
        The output of this code is the “. thread is running...” output that 4 threads will print,
        but the 5th thread cannot enter because there is no space in the multiplex and therefore cannot output.

        !! To run the correct program, put any thread in the comment line.

        if you always get the output in the order 1, 2, 3, 4, 5, it could mean that the threads are being executed sequentially
        due to how the Python interpreter or the Global Interpreter Lock (GIL) handles threading, especially for such simple code.
        In Python, the GIL ensures that only one thread executes Python bytecode at a time.
        However, this does not mean threads will always execute in the same order.
"""

import threading

multiplex = threading.Semaphore(4)

def execute(thread_number):
    multiplex.acquire()
    """
        In this section, threads the size of multiplex (4) can run at the same time.
        If u run the program like this order, the code gets in the deadlock.
        Becuase "NO RELEASING".
        That proves the "threads the size of multiplex can run at the same time."
        Because when 4 thread get in the critical section, 5. thread has to wait.
    """
    print(f"{thread_number}. thread is running...")
    # TODO : For correct solution uncomment this line.
    # multiplex.release()


# 5 thread oluştur
thread1 = threading.Thread(target=execute, args=(1,))
thread2 = threading.Thread(target=execute, args=(2,))
thread3 = threading.Thread(target=execute, args=(3,))
thread4 = threading.Thread(target=execute, args=(4,))
thread5 = threading.Thread(target=execute, args=(5,))

# Thread'leri başlat
thread1.start()
thread2.start()
thread3.start()
thread4.start()
thread5.start()

# Tüm thread'lerin bitmesini bekle
thread1.join()
thread2.join()
thread3.join()
thread4.join()
thread5.join()
