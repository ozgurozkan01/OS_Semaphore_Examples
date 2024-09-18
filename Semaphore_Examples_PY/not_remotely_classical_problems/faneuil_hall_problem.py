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

            - NOTE !!
                In this solution it is possible for immigrants to get stuck, after they
                get their certificate, by another judge coming to swear in the next batch of
                immigrants. If that happens, they might have to wait through another swearing
                in-ceremony.
"""

# Constants
immigrant_count = 5
spectator_count = 3

# Shared variables
no_judge = threading.Lock()  # acts as a turnstile for immigrants and spectators
entered_immigrant = 0  # counts the number of immigrants in the room
checked_immigrant = 0  # counts the number of immigrants who have checked in
mutex = threading.Lock()  # protects checked value control
confirmed = threading.Semaphore(0)  # signals that the judge has executed confirm
all_signed_in = threading.Semaphore(0)
is_judge = False


def execute_immigrant():

    """
                Immigrants pass through a turnstile when they enter; while the judge is in
                the room, the turnstile is locked.
                After entering, immigrants have to get mutex to check in and update
                checked. If there is a judge waiting, the last immigrant to check in signals
                allSignedIn and passes the mutex to the judge
    """

    global entered_immigrant, checked_immigrant, is_judge

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
    print("Immigrant sweared...")  # swear()
    print("Immigrant got certificate...")  # get_certificate()

    no_judge.acquire()
    print("Immigrant is leaving...")  # leave()
    no_judge.release()


def execute_judge():

    """
                The judge holds noJudge to bar immigrants and spectators from entering,
                and mutex so he can access entered and checked.

                If the judge arrives at an instant when everyone who has entered has also
                checked in, she can proceed immediately. Otherwise, she has to give up the
                mutex and wait. When the last immigrant checks in and signals allSignedIn,
                it is understood that the judge will get the mutex back.

                After invoking confirm, the judge signals confirmed once for every immigrant who has checked in,
                and then resets the counters (an example of “I’ll do it for you”).
                Then the judge leaves and releases mutex and noJudge.

                After the judge signals confirmed, immigrants invoke swear and
                getCertificate concurrently, and then wait for the noJudge turnstile to open
                before leaving.
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
    checked_immigrant = 0

    print("Judge is leaving the room...")  # leave()
    is_judge = False
    mutex.release()
    no_judge.release()


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