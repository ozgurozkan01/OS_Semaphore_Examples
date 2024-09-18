import threading
from threading import Semaphore
import time

"""
            - CONSTRAINTS ABOUT PROBLEM !!
                1. Any number of students can be in a room at the same time.
                2. The Dean of Students can only enter a room if there are no students in
                the room (to conduct a search) or if there are more than 50 students in
                the room (to break up the party).
                3. While the Dean of Students is in the room, no additional students may
                enter, but students may leave.
                4. The Dean of Students may not leave the room until all students have left.
                5. There is only one Dean of Students, so you do not have to enforce exclusion
                among multiple deans.
                
            - CODE OUTPUT !!
             
"""

# Initialization
students = 0
dean = 'not_here'  # can be 'waiting' or 'in_the_room'
mutex = Semaphore(1)  # mutex for protecting students and dean
turn = Semaphore(1)  # turnstile to prevent students from entering while the Dean is in the room
clear = Semaphore(0)  # Ensures Dean leaves after all students have left
lie_in = Semaphore(0)  # Rendezvous between student and the Dean

min_students_dean_enter_room = 50

def execute_dean():
    global students, dean
    while True:

        """
                When the Dean arrives, there are three cases: if there are students in the
                room, but not 50 or more, the Dean has to wait. If there are 50 or more, the
                Dean breaks up the party and waits for the students to leave. If there are no
                students, the Dean searches and leaves.

                In the first two cases, the Dean has to wait for a rendezvous with a student,
                so he has to give up mutex to avoid a deadlock. When the Dean wakes up, he
                has to modify the scoreboard, so he needs to get the mutex back.
        """

        mutex.acquire()

        if students > 0 and students < min_students_dean_enter_room:
            dean = 'waiting'
            print("Dean is waiting at the party...")
            mutex.release()
            lie_in.acquire()  # Get the mutex from the student

        # Students must be either 0 or >= 50
        if students >= min_students_dean_enter_room:
            dean = 'in_the_room'
            print("Dean is breaking up the party...")
            turn.acquire()  # Lock the turnstile
            mutex.release()
            clear.acquire()  # Get the mutex from the last student leaving
            turn.release()  # Unlock the turnstile
        else:  # Students must be 0
            print("Dean is searching the room...")
            time.sleep(1)  # Simulate the search
            print("Dean leaves the room...")

        dean = 'not_here'
        mutex.release()


def execute_student(student_code):
    global students, dean
    while True:

        """
                There are three cases where a student might have to signal the Dean. If
                the Dean is waiting, then the 50th student in or the last one out has to signal
                lieIn. If the Dean is in the room (waiting for all the students to leave), the last
                student out signals clear. In all three cases, it is understood that the mutex
                passes from the student to the Dean.

                One part of this solution that may not be obvious is how we know at Dean’s code
                that students must be 0 or not less than 50. The key is to realize that there are
                only two ways to get to this point: either the first conditional was false,
                which means that students is either 0 or not less than 50; or the Dean was waiting on
                lieIn when a student signaled, which means, again, that students is either 0 or not less than 50
        """

        mutex.acquire()

        if dean == 'in_the_room':
            mutex.release()
            turn.acquire()  # Wait for the Dean to leave
            turn.release()
            mutex.acquire()

        students += 1

        if students == min_students_dean_enter_room and dean == 'waiting':
            lie_in.release()  # Signal the Dean that 50 students have arrived
        else:
            mutex.release()

        print(f"Student {student_code} is partying...")

        mutex.acquire()

        students -= 1

        if students == 0 and dean == 'waiting':
            lie_in.release()  # Signal the Dean that all students have left
        elif students == 0 and dean == 'in_the_room':
            clear.release()  # Signal the Dean that all students have left the room
        else:
            mutex.release()


student_count = 250
students_threads = []

for i in range(student_count):
    student_thread = threading.Thread(target=execute_student, args=(i,))
    students_threads.append(student_thread)
    student_thread.start()

dean_thread = threading.Thread(target=execute_dean)
dean_thread.start()

for student_thread in students_threads:
    student_thread.join()

dean_thread.join()
