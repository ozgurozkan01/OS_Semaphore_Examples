import threading

# A program might work correctly 1000 times in a row, and then crash on the 1001st run,
# depending on the particular decisions of the scheduler.

def thread_a():
    print('YES')

def thread_b():
    print('NO')

a = threading.Thread(target=thread_a,)
b = threading.Thread(target=thread_b,)

a.start()
b.start()

a.join()
b.join()