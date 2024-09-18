import threading
import time

"""

- PROBLEM DEFINITION !!
                This problem deals with the management of the right of way between two different groups on a passageway.
                The passageway is too narrow to allow two people to pass side by side, so when two people meet, only one of them can pass.

            - SCENARIO !!
                * Groups: There are two groups: Group A (the “heathens” from Modus Hall) and Group B (the “prudes” from West Hall).
                * Passageway: A narrow passageway is where the two groups meet. The passageway allows only one person to pass.
                * Critical Area: The passageway must be controlled between both groups based on majority rule.

            - CONDITIONS !!
            As each student checks in, he has to consider the following cases:
                • If the field is empty, the student lays claim for the heathens.
                • If the heathens currently in charge, but the new arrival has tipped the
                    balance, he locks the prude turnstile and the system switches to transition
                    mode.
                • If the prudes in charge, but the new arrival doesn’t tip the balance, he
                    joins the queue.
                • If the system is transitioning to heathen control, the new arrival joins the
                    queue.
                • Otherwise we conclude that either the heathens are in charge, or the system is transitioning to prude control. In either case, this thread can
                    proceed.

            Similarly, as each student checks out, she has to consider several cases.

                • If she is the last heathen to check out, she has to consider the following:
                    – If the system is in transition, that means that the prude turnstile is
                        locked, so she has to open it.
                    – If there are prudes waiting, she signals them and updates status so
                        the prudes are in charge. If not, the new status is ’neutral’.
                • If she is not the last heathen to check out, she still has to check the
                    possibility that her departure will tip the balance. In that case, she closes
                    the heathen turnstile and starts the transition.

            - ATTENTION !!
                One potential difficulty of this solution is that any number of threads could
                be interrupted at "Risk Line" , where they would have passed the turnstile but not yet
                checked in. Until they check in, they are not counted, so the balance of power
                may not reflect the number of threads that have passed the turnstile. Also,
                a transition ends when all the threads that have checked in have also checked
                out. At that point, there may be threads (of both types) that have passed the
                turnstile.

                These behaviors may affect efficiency—this solution does not guarantee maximum concurrency
                but they don’t affect correctness, if you accept that “majority rule” only applies to threads that have registered to vote.


            - CODE OUTPUT (In my IDE code runs without stucking on DEBUG mode)
                Heathen is crossing the field.
                Heathen is crossing the field.
                Heathen is crossing the field.
                Prude is crossing the field.
                Prude is crossing the field.
                Prude is crossing the field.
                Prude is crossing the field.
                Heathen is crossing the field.
                Heathen is crossing the field.
                Prude is crossing the field.
                --------- * -------- * --------- *

                Normal Running
                Heathen is crossing the field.
                Heathen is crossing the field.
                Prude is crossing the field.
                Prude is crossing the field.
                Prude is crossing the field.
                Prude is crossing the field.
                Prude is crossing the field.
                (STUCKING)

"""

class Status:
    NEUTRAL = 0
    HEATHENS_RULE = 1
    PRUDES_RULE = 2
    TRANSITION_TO_HEATHENS = 3
    TRANSITION_TO_PRUDES = 4


heathen_counter = 0
prude_counter = 0
field_status = Status.NEUTRAL

mutex = threading.Lock()
heathen_turn = threading.Lock()
prude_turn = threading.Lock()
heathen_queue = threading.Semaphore(0)
prude_queue = threading.Semaphore(0)


def execute_heathen():
    global heathen_counter, prude_counter, field_status

    # Wait for the turnstile
    with heathen_turn:
        pass

    # Risk Line
    with mutex:
        heathen_counter += 1
        if field_status == Status.NEUTRAL:
            field_status = Status.HEATHENS_RULE
        elif field_status == Status.PRUDES_RULE:
            if heathen_counter > prude_counter:
                field_status = Status.TRANSITION_TO_HEATHENS
                prude_turn.acquire()
        elif field_status == Status.TRANSITION_TO_HEATHENS:
            pass  # Wait for turn

    heathen_queue.acquire()  # Wait in queue
    print("Heathen is crossing the field.")
    time.sleep(0.5)  # Simulate crossing time

    with mutex:
        heathen_counter -= 1
        if heathen_counter == 0:
            if field_status == Status.TRANSITION_TO_PRUDES:
                prude_turn.release()
            if prude_counter > 0:
                prude_queue.release(prude_counter)
                field_status = Status.PRUDES_RULE
            else:
                field_status = Status.NEUTRAL
        if field_status == Status.HEATHENS_RULE and prude_counter > heathen_counter:
            field_status = Status.TRANSITION_TO_PRUDES
            heathen_turn.acquire()


def execute_prude():
    global heathen_counter, prude_counter, field_status

    # Wait for the turnstile
    with prude_turn:
        pass

    # Risk Line
    with mutex:
        prude_counter += 1
        if field_status == Status.NEUTRAL:
            field_status = Status.PRUDES_RULE
        elif field_status == Status.HEATHENS_RULE:
            if prude_counter > heathen_counter:
                field_status = Status.TRANSITION_TO_PRUDES
                heathen_turn.acquire()
        elif field_status == Status.TRANSITION_TO_PRUDES:
            pass  # Wait for turn

    prude_queue.acquire()  # Wait in queue
    print("Prude is crossing the field.")
    time.sleep(0.5)  # Simulate crossing time

    with mutex:
        prude_counter -= 1
        if prude_counter == 0:
            if field_status == Status.TRANSITION_TO_HEATHENS:
                heathen_turn.release()
            if heathen_counter > 0:
                heathen_queue.release(heathen_counter)
                field_status = Status.HEATHENS_RULE
            else:
                field_status = Status.NEUTRAL
        if field_status == Status.PRUDES_RULE and heathen_counter > prude_counter:
            field_status = Status.TRANSITION_TO_HEATHENS
            prude_turn.acquire()


def run():
    threads = []
    # Start heathen and prude threads
    for _ in range(5):
        threads.append(threading.Thread(target=execute_heathen))
        threads.append(threading.Thread(target=execute_prude))

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()


if __name__ == "__main__":
    run()
