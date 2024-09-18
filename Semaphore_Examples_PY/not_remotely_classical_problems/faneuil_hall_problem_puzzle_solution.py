import threading

"""
            - PROBLEM DEFINITION !!
                “There are three kinds of threads: immigrants, spectators, and a one judge.
                Immigrants must wait in line, check in, and then sit down. At some point, the
                judge enters the building. When the judge is in the building, no one may enter,
                and the immigrants may not leave. Spectators may leave. Once all immigrants
                check in, the judge can confirm the naturalization. After the confirmation, the
                immigrants pick up their certificates of U.S. Citizenship. The judge leaves at
                some point after the confirmation. Spectators may now enter as before. After
                immigrants get their certificates, they may leave.”

            - CONSTRAINTS !!
                • Immigrants must invoke enter, checkIn, sitDown, swear,
                    getCertificate and leave.
                • The judge invokes enter, confirm and leave.
                • Spectators invoke enter, spectate and leave.
                • While the judge is in the building, no one may enter and immigrants may
                    not leave.
                • The judge can not confirm until all immigrants who have invoked enter
                    have also invoked checkIn.
                • Immigrants can not getCertificate until the judge has executed
                    confirm
                    
            - SOLUTION WAY !!
            To solve the puzzle, we need to ensure that after the judge leaves, all the immigrants
            who have been sworn in must leave before the judge can enter again. This requires us
            to track the number of immigrants who have sworn in but not yet left.
            The judge cannot re-enter until all such immigrants have exited.

            We can achieve this by introducing an additional counter that keeps track of how many immigrants are still in the building.
            The judge will wait until this counter reaches zero before being allowed to enter again.
"""

# Constants
immigrant_count = 5
spectator_count = 3

# Shared variables
no_judge = threading.Lock()  # Acts as a turnstile for immigrants and spectators
entered_immigrant = 0  # Counts the number of immigrants in the room
checked_immigrant = 0  # Counts the number of immigrants who have checked in
remaining_immigrants = 0  # Counts the number of immigrants who haven't left after being sworn in
mutex = threading.Lock()  # Protects counters
confirmed = threading.Semaphore(0)  # Signals that the judge has executed confirm
all_signed_in = threading.Semaphore(0)
all_immigrants_left = threading.Semaphore(1)  # Signals that all immigrants have left
is_judge = False


def execute_immigrant():
    global entered_immigrant, checked_immigrant, remaining_immigrants, is_judge

    # Immigrants cannot enter while the judge is in the building
    no_judge.acquire()
    print("Immigrant entered the room...")  # enter()
    entered_immigrant += 1
    remaining_immigrants += 1  # Increment remaining immigrants in the building
    no_judge.release()

    mutex.acquire()
    print("Immigrant checked in...")  # checkIn()
    checked_immigrant += 1

    if is_judge and entered_immigrant == checked_immigrant:
        all_signed_in.release()
    else:
        mutex.release()

    print("Immigrant had a seat...")  # sitDown()
    confirmed.acquire()
    print("Immigrant swore...")  # swear()
    print("Immigrant got certificate...")  # getCertificate()

    no_judge.acquire()
    print("Immigrant is leaving...")  # leave()
    remaining_immigrants -= 1  # Decrement remaining immigrants
    if remaining_immigrants == 0:
        # If no immigrants are left, signal that the building is clear
        all_immigrants_left.release()
    no_judge.release()


def execute_judge():
    global entered_immigrant, checked_immigrant, is_judge

    # The judge must wait for all sworn-in immigrants to leave before entering
    all_immigrants_left.acquire()

    no_judge.acquire()
    mutex.acquire()

    print("Judge entered the room...")  # enter()
    is_judge = True

    if entered_immigrant > checked_immigrant:
        mutex.release()
        all_signed_in.acquire()

    print("Judge confirmed all checked...")  # confirm()
    for _ in range(checked_immigrant):
        confirmed.release()

    entered_immigrant = 0
    checked_immigrant = 0

    print("Judge is leaving the room...")  # leave()
    is_judge = False
    mutex.release()
    no_judge.release()

    # The judge cannot re-enter until all immigrants have left
    all_immigrants_left.release()


def execute_spectator():
    no_judge.acquire()
    print("Spectator entered the room...")  # enter()
    no_judge.release()

    print("Spectator is spectating...")  # spectate()
    print("Spectator is leaving...")  # leave()


threads = []

for _ in range(immigrant_count):
    threads.append(threading.Thread(target=execute_immigrant))
threads.append(threading.Thread(target=execute_judge))
for _ in range(spectator_count):
    threads.append(threading.Thread(target=execute_spectator))

for t in threads:
    t.start()

for t in threads:
    t.join()