import threading
import time

"""
            THAT IS THE SOLUTION 1 !!

            The only reason a waiting customer has to reacquire the mutex is to update the
            state of eating and waiting, so one way to solve the problem is to make the
            departing customer, who already has the mutex, do the updating

            - CODE OUTPUT !!
                ...
                ...
                Thread 10152 is eating sushi.
                Eating count: 5
                Thread 876 is eating sushi.
                Eating count: 5Eating count: 5Thread 21024 is eating sushi.
                
                Thread 18764 is eating sushi.
                
                Thread 24648 is eating sushi.
                
                Eating count: 5Eating count: 5
                Eating count: 5
                Thread 9752 is eating sushi.Thread 17620 is eating sushi.
                
                Eating count: 5
                Thread 24032 is eating sushi.
                Thread 9800 is eating sushi.
                
                Eating count: 5
                Thread 18576 is eating sushi.
                Eating count: 5Eating count: 5
                Eating count: 5
                Thread 10096 is eating sushi.
                Eating count: 5
                Thread 24336 is eating sushi.
                Eating count: 5
                Thread 5384 is eating sushi.
                Thread 24424 is eating sushi.
                ...
                ...

"""

# Constants
seat_amount = 5
eating_counter = 0  # Keeps track of the number of threads sitting at the bar
waiting_counter = 0  # Keeps track of the number of threads waiting
mutex = threading.Lock()  # Protects both counters
block = threading.Semaphore(0)  # Semaphore to block incoming customers if the bar is full
must_wait = False  # Indicates that the bar is (or has been) full


def execute():
    global eating_counter, waiting_counter, must_wait

    # Lock the mutex to protect shared variables
    mutex.acquire()

    if must_wait:
        waiting_counter += 1
        mutex.release()  # Release the lock while waiting
        block.acquire()  # Block until seats are available
    else:
        eating_counter += 1
        must_wait = eating_counter == seat_amount
        mutex.release()  # Release the lock after modifying the counter

    print(f"Eating count: {eating_counter}")
    print(f"Thread {threading.get_ident()} is eating sushi.")

    # Simulate the time spent eating sushi
    time.sleep(0.5)

    # Lock the mutex again to modify the counters
    mutex.acquire()
    eating_counter -= 1

    if eating_counter == 0:
        # Allow waiting customers to enter
        signal_count = min(5, waiting_counter)
        waiting_counter -= signal_count
        eating_counter += signal_count
        must_wait = eating_counter == seat_amount
        for _ in range(signal_count):
            block.release()

    mutex.release()


customer_count = 70
threads = []

# Create and start threads for customers
for _ in range(customer_count):
    t = threading.Thread(target=execute)
    threads.append(t)
    t.start()

# Join all threads to ensure they complete before finishing the program
for t in threads:
    t.join()