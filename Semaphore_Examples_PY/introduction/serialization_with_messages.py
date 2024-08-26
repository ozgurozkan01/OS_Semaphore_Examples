import threading

"""
        Serialization : Event A must happen before Event B

        Thread A (You)          Thread B (Bob)
        1 Eat breakfast         1 Eat breakfast
        2 Work                  2 Wait for a call
        3 Eat lunch             3 Eat lunchS
        4 Call Bob

        !!!!!
        It doesn't matter when Thread B has breakfast.
        He can do it before A or after. But he must always have lunch after A.

        Eeating breakfast concurrently but lunch sequentially

        - CODE OUTPUT !!
        This code will run thread independent until bob calls himself, and thread dependent for bob to have lunch.
        The output here is that Calling Bob, Received call from bob, eating lunch will happen in order.
        Otherwise the code output is incorrect.
"""

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

