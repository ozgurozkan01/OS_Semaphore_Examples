import threading


"""
            THAT IS THE SOLUTION 2 !!

            Alternatively, solution 2 is based on the counter-intuitive idea that a lock can be transferred from one thread to another thread!
            In other words, one thread can acquire a lock and then another thread can release it.
            This is fine as long as both threads understand that the lock has been transferred.
"""

# Constants
seat_amount = 5
eating_counter = 0  # Keep track of the number of threads sitting at the bar
waiting_counter = 0  # Keep track of the number of threads waiting
mutex = threading.Lock()  # Mutex protects both counters
block = threading.Semaphore(0)  # Some incoming customers have to block on block
must_wait = False  # Must wait indicates that the bar is (or has been) full


def execute():
    global eating_counter, waiting_counter, must_wait

    # Lock the mutex
    mutex.acquire()

    if must_wait:
        waiting_counter += 1
        mutex.release()
        block.acquire()
        waiting_counter -= 1

    eating_counter += 1
    must_wait = eating_counter == seat_amount

    # If there are customers waiting and the bar is not full, release a waiting customer
    if waiting_counter and not must_wait:
        block.release()
    else:
        mutex.release()

    print(f"Eating count: {eating_counter}")
    print(f"Thread {threading.get_ident()} is eating sushi")

    # Simulate eating time
    mutex.acquire()
    eating_counter -= 1

    # If there are no more customers eating, allow waiting customers to enter
    if eating_counter == 0:
        must_wait = False

    # If there are customers waiting and the bar is not full, release one
    if waiting_counter and not must_wait:
        block.release()
    else:
        mutex.release()


customer_count = 70
threads = []

# Create and start threads for customers
for _ in range(customer_count):
    t = threading.Thread(target=execute)
    threads.append(t)
    t.start()

# Join all threads to ensure they finish before the program terminates
for t in threads:
    t.join()