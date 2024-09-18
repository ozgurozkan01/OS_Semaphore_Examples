import threading

"""
            - PROBLEM DEFINITION !!
                Students in the dining hall invoke dine and then leave. After invoking dine
                and before invoking leave a student is considered “ready to leave”.
                The synchronization constraint that applies to students is that, in order to
                maintain the illusion of social suave, a student may never sit at a table alone. A
                student is considered to be sitting alone if everyone else who has invoked dine
                invokes leave before she has finished dine

                If you analyze the constraints, you will realize that there is only one situation
                where a student has to wait, if there is one student eating and one student who
                wants to leave. But there are two ways to get out of this situation: another
                student might arrive to eat, or the dining student might finish.
                In either case, the student who signals the waiting student updates the
                counters, so the waiting student doesn’t have to get the mutex back. This
                is another example of the the “I’ll do it for you” pattern.

                When is student is checking in, if she sees one student eating and one waiting
                to leave, she lets the waiter off the hook and decrements readyToLeave for him.

                After dining, the student checks three cases:
                • If there is only one student left eating, the departing student has to give
                    up the mutex and wait.
                • If the departing student finds that someone is waiting for her, she signals
                    him and updates the counter for both of them.
                • Otherwise, she just decrements readyToLeave and leaves.
"""

# Number of students
student_count = 7

# Shared variables
eating_counter = 0
ready_to_leave_counter = 0

# Locks and semaphores
mutex = threading.Lock()
ok_to_leave = threading.Semaphore(0)

def execute(student_code):
    global eating_counter, ready_to_leave_counter

    print(f"{student_code}. student is getting food...")

    mutex.acquire()
    eating_counter += 1

    if eating_counter == 2 and ready_to_leave_counter == 1:
        ok_to_leave.release()
        ready_to_leave_counter -= 1

    mutex.release()

    print(f"{student_code}. student is dining...")

    mutex.acquire()
    eating_counter -= 1
    ready_to_leave_counter += 1

    if eating_counter == 1 and ready_to_leave_counter == 1:
        mutex.release()
        ok_to_leave.acquire()
    elif eating_counter == 0 and ready_to_leave_counter == 2:
        ok_to_leave.release()
        ready_to_leave_counter -= 2
        mutex.release()
    else:
        ready_to_leave_counter -= 1
        mutex.release()

    print(f"{student_code}. student is leaving...")


threads = []

for i in range(student_count):
    thread = threading.Thread(target=execute, args=(i,))
    threads.append(thread)
    thread.start()

for thread in threads:
    thread.join()