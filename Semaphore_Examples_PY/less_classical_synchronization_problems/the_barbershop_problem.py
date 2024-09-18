import threading
import time

"""
     - LOGIC OF RUNNING !!
        • Customer threads should invoke a function named getHairCut.
        • If a customer thread arrives when the shop is full, it can invoke balk, which does not return.
        • The barber thread should invoke cutHair.
        • When the barber invokes cutHair there should be exactly one thread invoking getHairCut concurrently.

     - CODE OUTPUT !!
      while code is running, the output can be seen correct. But it is not. Let's check it.

        - Code piece from output :
            Barber cut the customers hair...
            10860 customer got hair cut..
            11320 customer is balked..
            23216 customer is balked..
            Barber cut the customers hair...9132 customer got hair cut..
            
            Barber cut the customers hair...
            9132 customer got hair cut..
            Barber cut the customers hair...
            9132 customer got hair cut..

     - ATTENTION !!
       As u see, no guarantee the barber is gonna cut the customers' hairs in order. So that is the problem.
       Up to n customers can pass the turnstile, signal customer and wait on barber.
       When the barber signal barber, any of the customers might proceed.

    - SOLUTION !!
        FIFO BARBERSHOP PROBLEM...
"""

n = 4  # total number of customers, 3 in waiting room, 1 in chair
customer_counter = 0  # number of customers in the shop
mutex = threading.Lock()
barber = threading.Semaphore(0)  # there is just one barber
customer = threading.Semaphore(0)  # customer who is shaving
barber_done = threading.Semaphore(0)  # signals to the customer when barber is done.
customer_done = threading.Semaphore(0)  # signals to the barber when customer is done.

def execute_customer():
    global customer_counter
    while True:
        mutex.acquire()
        if customer_counter == n:
            mutex.release()
            print(f"{threading.get_ident()} customer is balked..")  # balk();
            return
        customer_counter += 1
        mutex.release()

        customer.release()
        barber.acquire()

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
        barber.release()
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