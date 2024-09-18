import threading
import time
from lightswitch import Lightswitch

"""
                - WITH TURNSTILE vs WITHOUT TURNSTILE !!
                When a turnstile is used, men and women are forced to enter the bathroom in turn.
                The first gender to arrive first gets a turn to enter the bathroom and
                the other gender is blocked from entering the bathroom until they leave.

                When the turnstile is not used, men and women may try to enter the bathroom
                at the same time using their own semaphores, which can sometimes lead to race conditions.

                - CODE OUTPUT !!
                    A female has entered the bathroom
                    A male has entered the bathroom
                    A male has entered the bathroom
                    A female has entered the bathroom
                    A male has entered the bathroom
                    A male has entered the bathroom
                    A female has entered the bathroom
                    A female has entered the bathroom
                    A female has entered the bathroom
                    A male has entered the bathroom
                    A female has entered the bathroom
                    A male has entered the bathroom
                    A male has entered the bathroom
                    A female has entered the bathroom
                    ...
                    ...

"""

# GLOBAL VARIABLES
empty = threading.Semaphore(1)  # Represents the room being empty (1: unlocked, 0: locked)
male_switch = Lightswitch()
female_switch = Lightswitch()
male_multiplex = threading.Semaphore(3)  # Up to 3 men can enter at the same time
female_multiplex = threading.Semaphore(3)  # Up to 3 women can enter at the same time
turnstile = threading.Lock()  # Ensures only one gender can access the bathroom at a time

def execute_male():
    while True:
        with turnstile:
            male_switch.lock(empty)

        male_multiplex.acquire()
        # Bathroom code here
        print("A male has entered the bathroom")
        male_multiplex.release()

        male_switch.unlock(empty)
        time.sleep(0.5)


def execute_female():
    while True:
        with turnstile:
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