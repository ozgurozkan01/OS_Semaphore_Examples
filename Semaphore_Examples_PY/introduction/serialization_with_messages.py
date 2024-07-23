import threading

def thread_a(event):
    print("Thread A: Eat breakfast")
    print("Thread A: Work")
    print("Thread A: Eat lunch")
    print("Thread A: Call Bob")
    event.set()


def thread_b(event):
    print("Thread B: Eat breakfast")
    event.wait() # when event is set, waiting ends
    print("Thread B: Eat lunch")


event = threading.Event() # To provide condition that BOB has to be called to eat lunch

a = threading.Thread(target=thread_a, args=(event,))
b = threading.Thread(target=thread_b, args=(event,))

a.start()
b.start()

a.join()
b.join()

