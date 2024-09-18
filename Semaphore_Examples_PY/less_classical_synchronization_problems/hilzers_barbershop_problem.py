import threading
import time
from queue import Queue
from threading import Semaphore

"""
        - LOGIC OF RUNNING !!
            • Customers invoke the following functions in order: enterShop, sitOnSofa, getHairCut, pay.
            • Barbers invoke cutHair and acceptPayment.
            • Customers cannot invoke enterShop if the shop is at capacity.
            • If the sofa is full, an arriving customer cannot invoke sitOnSofa.
            • When a customer invokes getHairCut there should be a corresponding barber executing cutHair concurrently, and vice versa.
            • It should be possible for up to three customers to execute getHairCut concurrently, and up to three barbers to execute cutHair concurrently.
            • The customer has to pay before the barber can acceptPayment.
            • The barber must acceptPayment before the customer can exit.

        - CODE OUTPUT !!
            All threads run synchronously with each other and operations happen simultaneously.
            Therefore, after a customer1 walks in, customer2 can sit on the barber's chair
            before customer1 can sit on the table. Therefore the output can look confusing
            because there are too many customers in the code.

                5692. customer entered the barbershop..
                4368. customer entered the barbershop..
                15908. customer entered the barbershop..
                22080. customer entered the barbershop..
                17860. customer entered the barbershop..
                5368. customer entered the barbershop..
                20872. customer entered the barbershop..
                22456. customer entered the barbershop..
                8076. customer entered the barbershop..
                8764. customer entered the barbershop..
                20652. customer entered the barbershop..
                10796. customer entered the barbershop..
                22148. customer entered the barbershop..
                16936. customer entered the barbershop..
                17436. customer entered the barbershop..
                5320. customer entered the barbershop..
                23272. customer entered the barbershop..
                22876. customer entered the barbershop..
                11352. customer entered the barbershop..
                21336. customer entered the barbershop..
"""

n = 20  # capacity
customer_counter = 0
mutex = threading.Lock()
sofa = Semaphore(4)  # Sofa capacity counter
customer1 = Semaphore(0)
customer2 = Semaphore(0)
barber = Semaphore(0)
payment = Semaphore(0)
receipt = Semaphore(0)

queue1 = Queue()
queue2 = Queue()

def execute_customer():
    global customer_counter
    while True:
        s1 = Semaphore(0)  # Create a shared pointer to sem
        s2 = Semaphore(0)  # Create a shared pointer to sem

        mutex.acquire()
        if customer_counter == n:
            # If barbershop is full.
            mutex.release()
            print(f"{threading.get_ident()}. customer is balked..")  # balk();
            return
        customer_counter += 1  # barbershop is not full and a customer entered.
        queue1.put(s1)  # add this_thread own semaphore into the queue.
        mutex.release()

        print(f"{threading.get_ident()}. customer entered the barbershop..")  # enterShop();
        time.sleep(1)

        customer1.release()  # Signal that a customer is waiting to be served. This allows the barber to start processing.
        s1.acquire()  # Wait till the barber is ready for shaving. When barber ready, he releases the semaphore.

        sofa.acquire()  # Customer who entered in the barbershop sat on sofa.

        print(f"{threading.get_ident()}. customer sat on sofa..")  # sitOnSofa();
        time.sleep(1)

        s1.release()  # Release the semaphore s1 after the customer has been served (sat on the sofa).
        mutex.acquire()
        queue2.put(s2)
        mutex.release()
        customer2.release()  # Signal that this customer is now ready to be served in the barber chair.
        s2.acquire()  # Wait till the barber is ready for this customer (seated in the barber chair).
        sofa.release()

        print(f"{threading.get_ident()}. customer sit in barber chair..")  # sitInBarberChair();
        time.sleep(1)

        print(f"{threading.get_ident()}. customer paid..")  # pay();
        time.sleep(1)

        payment.release()  # customer pay for shaving.
        receipt.acquire()  # wait till the barber accept the payment.

        mutex.acquire()
        customer_counter -= 1  # 1 customer is done. Here using the mutex because to provide the sync.
        mutex.release()

def execute_barber():
    while True:
        customer1.acquire()  # Wait till there is a customer who is ready to be served. This ensures the barber only works when there is a customer.
        mutex.acquire()
        s = queue1.get()
        s.release()  # Allow the customer to proceed to the sofa.
        s.acquire()  # Wait till the customer is seated on the sofa and is ready for the barber.
        mutex.release()
        s.release()  # Release the semaphore after ensuring the customer is ready.

        customer2.acquire()  # Wait till the customer is ready to get into the barber chair.
        mutex.acquire()
        s = queue2.get()  # Get the semaphore for the customer waiting in the barber chair queue (s2).
        mutex.release()
        s.release()  # Allow the customer to get into the barber chair.

        barber.release()

        print(f"{threading.get_ident()}. customer cut hair..")  # cutHair();
        time.sleep(1)

        payment.acquire()

        print("Barber accepted the payment")  # acceptPayment();
        time.sleep(1)

        receipt.release()


customers_thread = []
barbers_thread = []

for i in range(n + 2):  # More than n customers
    customers_thread.append(threading.Thread(target=execute_customer))

for i in range(3):
    barbers_thread.append(threading.Thread(target=execute_barber))

for thread in customers_thread:
    thread.start()

for thread in barbers_thread:
    thread.start()

for thread in customers_thread:
    thread.join()

for thread in barbers_thread:
    thread.join()