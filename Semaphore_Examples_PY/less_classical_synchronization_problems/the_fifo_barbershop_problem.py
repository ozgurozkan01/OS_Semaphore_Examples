import threading
import queue
import time

"""
     - PURPOSE of USING FIFO !!
        1. Sequential Service :
        The FIFO method ensures that customer service is provided in the order in which the customer enters the queue.
        This guarantees a fair delivery of services and ensures fairness with respect to any customer's entry into the queue.
        2. Fairness and Uniformity :
        FIFO ensures that each customer receives service according to the order in which they enter the queue.
        This allocates customers' waiting time to receive service fairly and any customer should be in front to receive service.

     - USAGE PRINCIPLE OF FIFO !!
        In the barbershop problem, the FIFO (First-In-First-Out) principle
        ensures that customers receive service in a fair and orderly manner.
        When customers enter the shop, they are sorted according to the FIFO
        order and added to a queue structure. This structure guarantees that
        the first customer to arrive is served first.

        Each customer has its own semaphore in the queue and is woken up by
        this semaphore when the barber is ready. The barber serves the
        customer at the head of the queue and releases the customer's
        semaphore when he is done. This arrangement ensures that services are
        rendered in an orderly and fair manner, keeps transactions organized
        and provides an efficient barber service.

     - CODE OUTPUT !!
        In previous solution, there is no guarantee the barber cut the customers' hair in order.
        In this solution, we use the queue and every thread is using its own semaphore.
        After its shaving is done, we pop the its semaphore from the queue. When its turn comes,
        then push its semaphore again into the queue. And that solves the synch problem.

        - Code piece of output !!
            Barber cut the customers hair...17880 customer got hair cut..
            
            14772 customer is balked..
            23220 customer is balked..
            Barber cut the customers hair...
            4336 customer got hair cut..
            Barber cut the customers hair...
            15936 customer got hair cut..
            Barber cut the customers hair...
            22388 customer got hair cut..
            Barber cut the customers hair...
            17880 customer got hair cut..
            Barber cut the customers hair...
            4336 customer got hair cut..
"""

class Sem:
    def __init__(self):
        self.own_sem = threading.Semaphore(0)

n = 4  # total number of customers, 3 in waiting room, 1 in chair
customer_counter = 0  # number of customers in the shop
mutex = threading.Lock()
# barber = threading.Semaphore(0)  # there is just one barber
customer = threading.Semaphore(0)  # customer who is shaving
barber_done = threading.Semaphore(0)  # signals to the customer when barber is done.
customer_done = threading.Semaphore(0)  # signals to the barber when customer is done.
customers_fifo = queue.Queue()  # solver the synch problem.

def execute_customer():
    global customer_counter
    while True:
        s = Sem()  # Create an instance of Sem

        mutex.acquire()
        if customer_counter == n:
            mutex.release()
            print(f"{threading.get_ident()} customer is balked..")  # balk();
            return
        customer_counter += 1
        customers_fifo.put(s)
        mutex.release()

        customer.release()
        s.own_sem.acquire()

        print(f"{threading.get_ident()} customer got hair cut..")  # getHairCut();
        time.sleep(1)

        customer_done.release()
        barber_done.acquire()

        mutex.acquire()
        customer_counter -= 1
        mutex.release()

def execute_barber():
    while True:
        customer.acquire()
        mutex.acquire()

        s = customers_fifo.get()

        mutex.release()

        s.own_sem.release()

        print("Barber cut the customers hair...")  # cutHair();
        time.sleep(1)

        customer_done.acquire()
        barber_done.release()


customers_thread = []
barber_thread = threading.Thread(target=execute_barber)

for i in range(n + 2):  # More than n customers
    customer_thread = threading.Thread(target=execute_customer)
    customers_thread.append(customer_thread)

barber_thread.start()

for customer_thread in customers_thread:
    customer_thread.start()

for customer_thread in customers_thread:
    customer_thread.join()

barber_thread.join()