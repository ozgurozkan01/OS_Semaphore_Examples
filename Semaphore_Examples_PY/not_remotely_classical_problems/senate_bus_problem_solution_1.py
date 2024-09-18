import threading
import time

"""
            - PROBLEM DEFINITION !!
                Riders come to a bus stop and wait for a bus. When the bus arrives, all the waiting
                riders invoke boardBus, but anyone who arrives while the bus is boarding has
                to wait for the next bus. The capacity of the bus is 50 people; if there are more
                than 50 people waiting, some will have to wait for the next bus.
                When all the waiting riders have boarded, the bus can invoke depart. If the
                bus arrives when there are no riders, it should depart immediately

            - PASS THE BATON PATTERN
                It is a synchronization model used in concurrent programming and multithreading environments.
                This model is used to delegate a resource or a critical region between threads.
                The goal is to enable one thread to efficiently delegate a resource or lock to another thread.

            - PATTERN PURPOSE
                The purpose of this pattern is to minimize the use of synchronization tools such as locks (mutex or semaphore)
                by delegating control or resources between threads. This avoids unnecessary holding of locks and reduces waiting times.
"""

# Constants
max_rider_count_per_bus = 50
total_bus_count = 3
total_rider_count = 50

# Shared variables
rider_count = 0
mutex = threading.Lock()  # Mutex for shared variables
multiplex = threading.Semaphore(max_rider_count_per_bus)  # Controls the waiting area size
bus = threading.Semaphore(0)  # Indicates if the bus has arrived
all_aboard = threading.Semaphore(0)  # Signals when all riders have boarded

def execute_bus():
    """
                 When the bus arrives, it gets mutex, which prevents late arrivals from entering the boarding area.
                 If there are no riders, it departs immediately. Otherwise, it signals bus and waits for the riders to board
    """
    global rider_count
    while True:
        mutex.acquire()
        if rider_count > 0:
            bus.release()  # Allow riders to start boarding
            all_aboard.acquire()  # Wait for all riders to board
        mutex.release()

        print("Bus is departing...\n")

def execute_rider(rider_id):
    """
    Riders wait for the bus to arrive. After the bus arrives, they increment the rider count,
    board the bus, and then decrement the count. The last rider signals the bus to depart.
    """
    global rider_count
    while True:
        multiplex.acquire()  # Control the number of riders in the boarding area
        mutex.acquire()
        rider_count += 1
        mutex.release()

        bus.acquire()  # Wait for the bus to arrive
        multiplex.release()  # Allow more riders in the waiting area

        print(f"Rider {rider_id} is boarding the bus...")

        rider_count -= 1

        if rider_count == 0:
            all_aboard.release()  # Signal that all riders are aboard
        else:
            bus.release()  # Allow the next rider to board


# Create and start bus threads
bus_threads = [threading.Thread(target=execute_bus) for _ in range(total_bus_count)]
for t in bus_threads:
    t.start()

# Create and start rider threads
rider_threads = [threading.Thread(target=execute_rider, args=(i,)) for i in range(total_rider_count)]
for t in rider_threads:
    t.start()

# Join all threads (wait for them to finish)
for t in bus_threads:
    t.join()
for t in rider_threads:
    t.join()