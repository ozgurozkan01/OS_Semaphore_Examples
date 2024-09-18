import threading

"""
            - PROBLEM DEFINITION !!
                Riders come to a bus stop and wait for a bus. When the bus arrives, all the waiting
                riders invoke boardBus, but anyone who arrives while the bus is boarding has
                to wait for the next bus. The capacity of the bus is 50 people; if there are more
                than 50 people waiting, some will have to wait for the next bus.
                When all the waiting riders have boarded, the bus can invoke depart. If the
                bus arrives when there are no riders, it should depart immediately

            - I'll DO IT FOR YOU PATTERN !!
                It is a pattern often used in parallel programming or synchronization scenarios,
                where a component that performs an operation must also take over the work that needs to be done on behalf of another component.

            - PATTERN PURPOSE !!
                The main purpose of this pattern is that a thread that completes a certain part of a job will take over the rest of the
                work that other threads need to do. This pattern can speed up the job completion process and avoid unnecessary synchronization waits.
"""

# Constants
max_rider_count_per_bus = 50
total_bus_count = 3
total_rider_count = 50

# Shared variables
waiting = 0  # Number of riders in the boarding area, protected by mutex
mutex = threading.Lock()  # Protects the waiting value
bus = threading.Semaphore(0)  # Signals when the bus has arrived
boarded = threading.Semaphore(0)  # Signals that a rider has boarded

def execute_bus():
    """
                The bus gets the mutex and holds it throughout the boarding process. The
                loop signals each rider in turn and waits for her to board. By controlling the
                number of signals, the bus prevents more than 50 riders from boarding.
                When all the riders have boarded, the bus updates waiting, which is an
                example of the “I’ll do it for you” pattern
    """
    global waiting
    while True:
        mutex.acquire()

        n = min(waiting, max_rider_count_per_bus)

        for _ in range(n):
            bus.release()
            boarded.acquire()

        waiting = max(waiting - max_rider_count_per_bus, 0)
        mutex.release()

        # depart()
        print("depart")

def execute_rider():
    """
    Each rider waits for the bus to arrive and boards when the bus signals.
    """
    global waiting
    while True:
        mutex.acquire()
        waiting += 1
        mutex.release()

        bus.acquire()
        # board()
        print("board")
        boarded.release()


bus_threads = [threading.Thread(target=execute_bus) for _ in range(total_bus_count)]
for t in bus_threads:
    t.start()

rider_threads = [threading.Thread(target=execute_rider) for _ in range(total_rider_count)]
for t in rider_threads:
    t.start()

for t in bus_threads:
    t.join()
for t in rider_threads:
    t.join()