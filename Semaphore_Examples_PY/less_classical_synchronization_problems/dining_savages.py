import threading
import time

# IN THIS EXAMPLE SCOREBOARD PATTERN IS USED !!

"""
 - LOGIC OF RUNNING !!
 The code ensures that savages can only eat when there are servings in the pot.
 If the pot is empty, the first savage to discover this will signal the cook to refill it,
 ensuring synchronization between the savages and the cook.

 - CODE OUTPUT !!
    Savage got the serving from pot..
    19236 savage is eating..
    
    Savage got the serving from pot..
    16808 savage is eating..
    Savage got the serving from pot..
    
    17736 savage is eating..
    
    Savage got the serving from pot..
    19236 savage is eating..
    
    Servings is put into pot by cook!!

    The output will repeat as savages continue to eat and the cook continues to refill the pot whenever it's empty.
    The specific thread IDs and order of actions may vary because the threads run concurrently.
"""

savage_number = 3
M = 5  # stands for serving amount
servings = 0
mutex = threading.Lock()
empty_pot = threading.Semaphore(0)  # It is binary semaphore, because we deal with just it is empty or not
full_pot = threading.Semaphore(0)   # It is binary semaphore, because we deal with just it is empty or not

def execute_savage():
    global servings
    while True:
        mutex.acquire()  # if a thread gets in, others have to wait till it is available.
        if servings == 0:  # No portions to eat means pot is empty
            empty_pot.release()  # pot is empty so deal with implementations when pot is empty
            full_pot.acquire()  # blocked to stop transactions that take place while the pot is full

            """
             Why savage is reset the servings ? if u r saying, I could not understand. Shouldn't the cook do it ?
             Because at the beginning of this problem,

             "When a savage wants to eat, he helps himself from the pot, unless it is empty. If the pot is
             empty, the savage wakes up the cook and then waits until the cook
             has refilled the pot."  is said. U r totally right, but there are a couple of reason of this.

             1- Access Consistency: Wild accesses the servings variable only when the mutex is received, which ensures that data is updated safely and prevents data races.

             2- Deadlock Risk Prevention: If the cook updated the servings variable,the cook would update the servings value after filling the pot and send fullPot. signal().
             In this case, the wild holding the mutex could wait for the cook to access the mutex, which could cause a deadlock.
            """
            servings = M  # set the portions as full

        servings -= 1  # decrease the pot
        print("Savage got the serving from pot..")  # get_serving_from_pot();
        mutex.release()
        print(f"{threading.get_ident()} savage is eating..\n")  # eat();

def execute_cook():
    while True:
        empty_pot.acquire()  # if pot is empty
        print("Servings is put into pot by cook!!\n")  # put_servings_in_pot(M);
        full_pot.release()  # arrange pot is full


savages = []

cook = threading.Thread(target=execute_cook)

for i in range(savage_number):
    savage = threading.Thread(target=execute_savage)
    savages.append(savage)

cook.start()

for savage in savages:
    savage.start()

cook.join()

for savage in savages:
    savage.join()