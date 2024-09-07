"""
        - LOGIC OF RUNNING !!

        Producer :
        A producer thread continuously generates an event and adds it to the buffer. After this addition,
        the mutex (item) is unlocked and access to the critical section is allowed.
        The items.release() function called afterwards signals the consumer to execute the event.
        The consumer cannot execute the event until this signal is given.

        Consumer :
        For items.acquire() to work, the value must be greater than 0. After acquire(), if the critical section is empty,
        the thread enters and pops an event from the buffer to execute. After the pop, it exits the critical section and releases the mutex and executes the event.

        - CODE OUTPUT !!

        Example Output:
        45. event is processsing
        97. event is processsing
        58. event is processsing
        30. event is processsing
        56. event is processsing
        72. event is processsing
        47. event is processsing
        52. event is processsing
        ...
"""

import threading
import queue
import random
import time

# Exclusive access to the buffer
mutex = threading.Semaphore(1)
# When items is positive, it indicates the number of items in the buffer.
# When it is negative, it indicates the number of consumer threads in queue
items = threading.Semaphore(0)
infinite_event_buffer = queue.Queue()

def wait_for_event():
    # Random Number Generator: Better than rand() implementation
    event = str(random.randint(1, 100))  # Random int number between 1-100
    return event

"""
    # REGULAR SOLUTION
    def producer_execute():
        # WaitForEvent
        event = wait_for_event()  # Local event
        mutex.acquire()
        # buffer.add(event)
        infinite_event_buffer.put(event)
        items.release()
        # items.release()
        mutex.release()
        time.sleep(1)
"""

"""
    - WHY IT IS BETTER !!!
    Performance:
    By releasing the mutex before signaling the semaphore, you avoid holding the mutex while notifying consumers,
    reducing the chance of unnecessary contention. This can lead to better performance
    because consumers can acquire the mutex without waiting for the producer to finish its operations.
    
    Efficiency:
    It avoids the scenario where a consumer is blocked waiting for the semaphore while the mutex is held by the producer.
    This helps in reducing the waiting time for consumers and can lead to more efficient execution.
"""
def improved_producer_execute():
    while True:
        # WaitForEvent
        event = wait_for_event()  # Local event
        mutex.acquire()
        # buffer.add(event)
        infinite_event_buffer.put(event)
        # items.release();
        mutex.release()
        # If we use items.release() outside of the mutex, that means improved producer solution
        items.release()
        time.sleep(2)

"""
    - WHY IT IS BROKEN !!
    
    Blocking with Mutex Held:
    By acquiring the mutex before waiting on the semaphore (items.acquire()),
    the consumer holds the mutex while it could potentially be blocked on the semaphore.
    This leads to inefficient use of the mutex and can cause other threads that need the mutex to be blocked unnecessarily.
    
    Reduced Efficiency:
    The holding of the mutex while waiting for an item to be available can lead to increased
    contention and potential delays, reducing the overall efficiency of the system.
    
    - BOOK EXPLANATION !!
    
    it can cause a deadlock. Imagine that the buffer is empty. A consumer arrives, gets the mutex, and then blocks on items.
    When the producer arrives, it blocks on mutex and the system comes to a grinding halt.
    This is a common error in synchronization code: any time you wait for a semaphore while holding a mutex,
    there is a danger of deadlock. When you are checking a solution to a synchronization problem, you should check for this kind of deadlock.

    def broken_consumer_execute():
        mutex.acquire()
        items.acquire()
        event = infinite_event_buffer.get()  # Local event
        mutex.release()
        process(event)
"""

def consumer_execute():
    while True:
        items.acquire()
        mutex.acquire()
        event = infinite_event_buffer.get()  # Local event
        mutex.release()
        process(event)
        time.sleep(2)

def process(event):
    print(f"{event} is processed...")


max_element_number = 3
producers = []
consumers = []

for _ in range(max_element_number):
    producer_thread = threading.Thread(target=improved_producer_execute)
    consumer_thread = threading.Thread(target=consumer_execute)
    producers.append(producer_thread)
    consumers.append(consumer_thread)

for producer in producers:
    producer.start()
for consumer in consumers:
    consumer.start()

for producer in producers:
    producer.join()
for consumer in consumers:
    consumer.join()