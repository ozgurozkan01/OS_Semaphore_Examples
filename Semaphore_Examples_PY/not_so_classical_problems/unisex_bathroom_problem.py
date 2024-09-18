import threading
import time
from lightswitch import Lightswitch

"""
            - PROBLEM DEFINITION !!
            For the comfort of employees in a company, it has been decided to place a unisex bathroom in a strategically convenient location.
            However, there are some constraints and conditions for the use of this bathroom.

            - CONSTRAINTS !!
                • There cannot be men and women in the bathroom at the same time.
                • There should never be more than three employees squandering company
                time in the bathroom.

            - ATTENTION !!
            At this problem, there is a starvation problem. So how ? If there are women in the bathroom - does not matter it is full or not-
            and also if there are men waiting for to get in in front of the door, men never might get in the bathroom. Because men and women
            cannot be in bathroom at the same time, when women are in the bathroom, if a queue of women forms at the door, men will never
            be able to enter the bathroom if there is at least one woman in the bathroom, and women waiting in the queue at the door will be able to enter in turn.

            - SOLUTION !!
            Using turnstile to prevent starvation.

            - CODE OUTPUT !!
                In the code output, while we should normally see a weighted female print as output, 
                we see an optimized code output due to the PYTHON language's own optimization support. 
                We cannot clearly see that this output contains starvation.
                
                
                A male has entered the bathroom
                A male has entered the bathroomA male has entered the bathroom
                
                A male has entered the bathroom
                A female has entered the bathroom
                A female has entered the bathroom
                A male has entered the bathroom
                A male has entered the bathroom
                A female has entered the bathroom
                A female has entered the bathroom
                A female has entered the bathroom
                A male has entered the bathroom
                A male has entered the bathroom
                A male has entered the bathroom
                A male has entered the bathroom
                A female has entered the bathroom
                A female has entered the bathroom
                A female has entered the bathroom
                A female has entered the bathroomA female has entered the bathroom
"""

# GLOBAL VARIABLES
empty = threading.Semaphore(1)  # Represents the room being empty (1: unlocked, 0: locked)
male_switch = Lightswitch()
female_switch = Lightswitch()
male_multiplex = threading.Semaphore(3)  # Up to 3 men can enter at the same time
female_multiplex = threading.Semaphore(3)  # Up to 3 women can enter at the same time

def execute_male():
    while True:
        male_switch.lock(empty)
        male_multiplex.acquire()
        # Bathroom code here
        print("A male has entered the bathroom")
        male_multiplex.release()
        male_switch.unlock(empty)
        time.sleep(0.5)

def execute_female():
    while True:
        female_switch.lock(empty)
        female_multiplex.acquire()
        # Bathroom code here
        print("A female has entered the bathroom")
        female_multiplex.release()
        female_switch.unlock(empty)
        time.sleep(0.5)


female_number = 5
male_number = 5
females = []
males = []

for _ in range(male_number):
    thread = threading.Thread(target=execute_male)
    males.append(thread)
    thread.start()

for _ in range(female_number):
    thread = threading.Thread(target=execute_female)
    females.append(thread)
    thread.start()

for thread in males:
    thread.join()

for thread in females:
    thread.join()