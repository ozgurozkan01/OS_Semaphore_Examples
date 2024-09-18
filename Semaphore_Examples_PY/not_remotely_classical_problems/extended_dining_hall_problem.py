import threading
import time

"""
            - PROBLEM DEFINITION !!
                The Dining Hall problem gets a little more challenging if we add another step.
                As students come to lunch they invoke getFood, dine and then leave. After
                invoking getFood and before invoking dine, a student is considered “ready to
                eat”. Similarly, after invoking dine a student is considered “ready to leave”.
                The same synchronization constraint applies: a student may never sit at a
                table alone. A student is considered to be sitting alone if either
                • She invokes dine while there is no one else at the table and no one ready
                to eat, or
                • everyone else who has invoked dine invokes leave before she has finished
                dine

                Again, if we analyze the constraints, we realize that there is only one situation
                where a student who is ready to eat has to wait, if there is no one eating and
                no one else ready to eat. And the only way out is if someone else arrives who is
                ready to eat.

            - PAY ATTENTION !!
                As in the previous solution, It is used the “I’ll do it for you” pattern so that a
                waiting student doesn’t have to get the mutex back.

                The primary difference between this solution and the previous one is that
                the first student who arrives at an empty table has to wait, and the second
                student allows both students to proceed. It either case, we don’t have to check
                for students waiting to leave, since no one can leave an empty table!
"""

# Constants
STUDENT_COUNT = 7

# Shared variables
ready_to_eat = 0
eating = 0
ready_to_leave = 0

# Semaphores and Mutex
ok_to_sit = threading.Semaphore(0)  # To signal when students can sit down
ok_to_leave = threading.Semaphore(0)  # To signal when students can leave
mutex = threading.Lock()  # Mutex for synchronization

def execute_student(student_code):
    global ready_to_eat, eating, ready_to_leave

    print(f"Student {student_code} is getting food...")

    mutex.acquire()
    ready_to_eat += 1

    if eating == 0 and ready_to_eat == 1:
        # If first student, they wait for another to join
        mutex.release()
        ok_to_sit.acquire()
    elif eating == 0 and ready_to_eat == 2:
        # If second student, both can sit down to eat
        ok_to_sit.release()
        ready_to_eat -= 2
        eating += 2
        mutex.release()
    else:
        ready_to_eat -= 1
        eating += 1
        if eating == 2 and ready_to_leave == 1:
            # Signal a waiting student to leave if applicable
            ok_to_leave.release()
            ready_to_leave -= 1
        mutex.release()

    print(f"Student {student_code} is dining...")

    time.sleep(1)  # Simulate dining time

    mutex.acquire()
    eating -= 1
    ready_to_leave += 1

    if eating == 1 and ready_to_leave == 1:
        # If there's one student eating and one ready to leave, the latter waits
        mutex.release()
        ok_to_leave.acquire()
    elif eating == 0 and ready_to_leave == 2:
        # If two are ready to leave and no one is eating, both can leave
        ok_to_leave.release()
        ready_to_leave -= 2
        mutex.release()
    else:
        ready_to_leave -= 1
        mutex.release()

    print(f"Student {student_code} is leaving...")

def run():
    threads = []

    for i in range(STUDENT_COUNT):
        thread = threading.Thread(target=execute_student, args=(i,))
        threads.append(thread)
        thread.start()

    for thread in threads:
        thread.join()

if __name__ == "__main__":
    run()
