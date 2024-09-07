import threading
import queue
import random
import time

# Exclusive access to the buffer
mutex = threading.Lock()
# When items is positive, it indicates the number of items in the buffer.
# When it is negative, it indicates the number of consumer threads in queue
items = threading.Semaphore(0)
buffer_size = 3
space = threading.Semaphore(buffer_size)
infinite_event_buffer = queue.Queue()

def wait_for_event():
    # Random Number Generator: Better than rand() implementation
    event = str(random.randint(1, 100))  # Random int number between 1-100
    return event

"""
    - Broken Finite Buffer Solution !!
    if (items >= bufferSize)
    {
        block();
    }
    
    This implementation does not work properly. Remember that we can’t check the current value of a semaphore
    
    - LOGIC OF RUNNING !!
       Compared to the previous example, this example is more fault tolerant and controlled because we control the buffer occupancy.
    
    - CODE OUTPUT !!
       Same output instance like in infinite producer-consumer problem
"""

def producer_execute():
    while True:
        # WaitForEvent
        event = wait_for_event()  # Local event
        space.acquire()
        mutex.acquire()
        # buffer.add(event)
        infinite_event_buffer.put(event)
        # items.release()
        mutex.release()
        # If we use items.release() outside of the mutex, that means improved producer solution
        items.release()
        time.sleep(1)

def consumer_execute():
    while True:
        items.acquire()
        mutex.acquire()

        # Bad Consumer Solution - DEADLOCK PROBLEM
        # If we use items.acquire() in the mutex, that means broken consumer solution. That causes deadlock problem.
        # items.acquire()

        event = infinite_event_buffer.get()  # Local event
        mutex.release()
        space.release()  # If we comment this line out, the program gets into deadlock when the buffer is full.

        process(event)
        time.sleep(1)

def process(event):
    print(f"{event} is processed...")

def run():
    max_element_number = 3
    producers = []
    consumers = []

    for _ in range(max_element_number):
        producer_thread = threading.Thread(target=producer_execute)
        consumer_thread = threading.Thread(target=consumer_execute)
        producers.append(producer_thread)
        consumers.append(consumer_thread)

    for p in producers:
        p.start()
    for c in consumers:
        c.start()

    for p in producers:
        p.join()
    for c in consumers:
        c.join()

if __name__ == "__main__":
    run()
