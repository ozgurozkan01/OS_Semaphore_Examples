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
"""

# Constants
immigrant_count = 5
spectator_count = 3

# Shared variables
no_judge = threading.Lock()  # Acts as a turnstile for immigrants and spectators
entered_immigrant = 0  # Counts the number of immigrants in the room
checked_immigrant = 0  # Counts the number of immigrants who have checked in
mutex = threading.Lock()  # Protects checked value control
confirmed = threading.Semaphore(0)  # Signals that the judge has executed confirm
all_signed_in = threading.Semaphore(0)
all_gone = threading.Semaphore(0)
exit_semaphore = threading.Semaphore(0)
is_judge = False


def execute_immigrant():

    """
                When the judge is ready to leave, she can’t release noJudge, because that would allow more immigrants,
                and possibly another judge, to enter. Instead, she signals exit, which allows one immigrant to leave, and passes mutex.

                The immigrant that gets the signal decrements checked and then passes the baton to the next immigrant.
                The last immigrant to leave signals allGone and passes the mutex back to the judge.
                This pass-back is not strictly necessary, but it has the nice feature that the judge releases both mutex
                and noJudge to end the phase cleanly
    """

    global entered_immigrant, checked_immigrant, is_judge

    # When the judge is ready to leave, she signals exit, which allows one immigrant to leave.
    no_judge.acquire()
    print("Immigrant entered the room...")  # enter()
    entered_immigrant += 1
    no_judge.release()

    mutex.acquire()
    print("Immigrant checked in...")  # check()
    checked_immigrant += 1

    if is_judge and entered_immigrant == checked_immigrant:
        all_signed_in.release()
    else:
        mutex.release()

    print("Immigrant had a seat...")  # sit_down()
    confirmed.acquire()
    print("Immigrant swore...")  # swear()
    print("Immigrant got certificate...")  # get_certificate()

    exit_semaphore.acquire()
    print("Immigrant is leaving the room...")  # leave()
    checked_immigrant -= 1

    if checked_immigrant == 0:
        all_gone.release()
    else:
        exit_semaphore.release()


def execute_judge():

    """
            The judge prevents any new entries while in the room by locking no_judge.
            The judge then locks mutex to check in the immigrants and wait until all are checked in.
            After confirming all checked-in immigrants, the judge signals them and resets counters.
            Finally, the judge releases the mutex and no_judge, allowing others to enter.
    """

    global entered_immigrant, checked_immigrant, is_judge

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

    print("Judge is leaving the room...")  # leave()
    is_judge = False

    exit_semaphore.release()
    all_gone.acquire()
    mutex.release()
    no_judge.release()


def execute_spectator():

    """
            Spectators enter the room without any restrictions and leave freely.
            They simply need to pass through the no_judge turnstile to enter and exit.
    """

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