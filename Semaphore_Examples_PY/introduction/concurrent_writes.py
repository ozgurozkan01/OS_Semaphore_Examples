import threading

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