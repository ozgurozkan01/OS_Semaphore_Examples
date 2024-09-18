import threading
import time

"""
         - LOGIC OF RUNNING !!
            Santa Claus sleeps in his shop at the North Pole and only wakes up on two occasions:

            * If all nine reindeer return from their South Pacific vacation.
            * If three elves are in trouble (in this case only Santa wakes up when three elves are in trouble).
            * If three elves are waiting for Santa's help, he waits until the other elves have had their turn.

            - When the ninth reindeer returns, Santa must prepare the sleigh and then all nine reindeer must make cattle.
            - When the third elf arrives, Santa must help the elves and all three elves must get help.
            - After the three elves have received help, the other elves can wait at the door of the shop.
            - Santa must work in a loop to be able to help many groups of elves. There are exactly nine reindeer, but the number of elves can change.

            - CODE OUTPUT !!
                20772 numbered elf will get help...
                19904 numbered elf will get help...
                17824 numbered elf will get help...
                Santa Claus helping elves...
                Santa Claus preparing sleigh...
                12832 numbered reindeer getting hitched...23248 numbered reindeer getting hitched...
                23416 numbered reindeer getting hitched...
                7076 numbered reindeer getting hitched...
                4368 numbered reindeer getting hitched...
                23044 numbered reindeer getting hitched...
                3060 numbered reindeer getting hitched...
                
                1192 numbered reindeer getting hitched...
                17696 numbered reindeer getting hitched...
                17824 numbered elf will get help...
                6708 numbered elf will get help...
                23532 numbered elf will get help...
"""

# Elves and reindeer constants
elves_number = 10
reindeer_number = 9

elf_counter = 0  # elves
reindeer_counter = 0  # reindeer
santaSem = threading.Semaphore(0)  # check the elf and reindeer counter
reindeerSem = threading.Semaphore(0)
elfTex = threading.Semaphore(1)
mutex = threading.Lock()


def execute_santa():
    global reindeer_counter
    while True:
        santaSem.acquire()
        mutex.acquire()

        if reindeer_counter == reindeer_number:
            print("Santa Claus preparing sleigh...")  # prepareSleigh()
            for _ in range(reindeer_number):
                reindeerSem.release()
            reindeer_counter = 0
        elif elf_counter == 3:
            print("Santa Claus helping elves...")  # helpElves()

        mutex.release()


def execute_reindeer():
    global reindeer_counter
    while True:
        mutex.acquire()
        reindeer_counter += 1
        if reindeer_counter == reindeer_number:
            santaSem.release()
        mutex.release()

        reindeerSem.acquire()
        print(f"{threading.get_ident()} numbered reindeer getting hitched...")  # getHitched()
        time.sleep(1)


def execute_elf():
    global elf_counter
    while True:
        elfTex.acquire()
        mutex.acquire()

        elf_counter += 1
        if elf_counter == 3:
            santaSem.release()
        else:
            elfTex.release()

        mutex.release()

        print(f"{threading.get_ident()} numbered elf will get help...")  # getHelp()
        time.sleep(1)

        mutex.acquire()
        elf_counter -= 1
        if elf_counter == 0:
            elfTex.release()

        mutex.release()

elves = []
reindeers = []
santa = threading.Thread(target=execute_santa)

for _ in range(elves_number):
    elf = threading.Thread(target=execute_elf)
    elves.append(elf)

for _ in range(reindeer_number):
    reindeer = threading.Thread(target=execute_reindeer)
    reindeers.append(reindeer)

santa.start()

for elf in elves:
    elf.start()

for reindeer in reindeers:
    reindeer.start()

santa.join()

for elf in elves:
    elf.join()

for reindeer in reindeers:
    reindeer.join()
