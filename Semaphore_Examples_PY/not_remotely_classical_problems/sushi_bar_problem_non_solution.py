import threading
import time

"""


            - PROBLEM DEFINITION !!
                Imagine a sushi bar with 5 seats. If you arrive while there is an empty seat, you can take a seat immediately.
                But if you arrive when all 5 seats are full, that means that all of them are dining together,
                and you will have to wait for the entire party to leave before you sit down

            - WHY THIS PROBLEM HAS NO SOLUTION !!
                First of all, do not think that it is a deadlock problem. Actually program runs and gives a solution,
                but this is not the result we want. The problem is, If a customer arrives while the bar is full,
                he has to give up the mutex while he waits so that other customers can leave.
                When the last customer leaves, she signals block, which wakes up at least some of the waiting customers,
                and clears must wait. But when the customers wake up, they have to get the mutex back,
                and that means they have to compete with incoming new threads. If new threads arrive and get the mutex first,
                they could take all the seats before the waiting threads. This is not just a question of injustice;
                it is possible for more than 5 threads to be in the critical section concurrently, which violates the synchronization constraints.

                In the "Little Book of Semaphore", 2 solutions were offered.

                1- one way to solve the problem is to make the departing customer, who already has the mutex, do the updating.
                2- one thread can acquire a lock and then another thread can release it.

            - CODE OUTPUT !!
                ...
                ...
                Thread 19568 is eating sushi.
                
                Eating count: 5
                Thread 21764 is eating sushi.
                Eating count: 1
                Thread 14212 is eating sushi.Eating count: 2
                Thread 10560 is eating sushi.
                Eating count: 3
                Thread 18928 is eating sushi.Eating count: 4
                
                Thread 24700 is eating sushi.
                
                Eating count: 5
                Thread 8012 is eating sushi.
                Eating count: 1
                Thread 5544 is eating sushi.Eating count: 2
                Thread 22960 is eating sushi.
                Eating count: 3
                
                Thread 21712 is eating sushi.Eating count: 4
                Thread 22408 is eating sushi.
                
                Eating count: 5
                Thread 6276 is eating sushi.
                Eating count: 1
                Thread 24924 is eating sushi.
                Eating count: 2
                Thread 5240 is eating sushi.Eating count: 3
                Thread 8720 is eating sushi.
                Eating count: 4
                ...
                ...

               As you can see from the output, although there are 5 seats, more than 5 customers can come and eat sushi.
"""

seat_amount = 5
eating_counter = 0  # keep track of the number of threads sitting at the bar
waiting_counter = 0  # keep track of the number of threads waiting
mutex = threading.Lock()  # mutex protects both counters
block = threading.Semaphore(0)  # some incoming customers have to block on block.
must_wait = False  # must wait indicates that the bar is (or has been) full

def execute():
    global eating_counter, waiting_counter, must_wait

    mutex.acquire()
    if must_wait:
        waiting_counter += 1
        mutex.release()
        block.acquire()

        mutex.acquire()
        waiting_counter -= 1

    eating_counter += 1
    print(f"Eating count: {eating_counter}")
    must_wait = eating_counter == seat_amount
    mutex.release()

    print(f"Thread {threading.get_ident()} is eating sushi.")
    time.sleep(0.5)

    mutex.acquire()
    eating_counter -= 1
    if eating_counter == 0:
        signal_count = min(seat_amount, waiting_counter)
        """
            Here, after calling block.release(signal_count);, waiting threads are expected to compete to get the mutex again.
            However, new threads can also try to get the mutex. If the new threads get the mutex before the waiting ones,
            then more than 5 threads can enter the eating section at once.
        """
        for _ in range(signal_count):
            block.release()
        must_wait = False
    mutex.release()

customer_count = 70
customers = []

for _ in range(customer_count):
    t = threading.Thread(target=execute)
    customers.append(t)
    t.start()

for t in customers:
    t.join()