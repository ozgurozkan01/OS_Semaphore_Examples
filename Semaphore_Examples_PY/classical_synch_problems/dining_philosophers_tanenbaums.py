import threading
from enum import Enum

'''
    - CONSTRAINTS !!
    • Only one philosopher can hold a fork at a time.
    • It must be impossible for a deadlock to occur.
    • It must be impossible for a philosopher to starve waiting for a fork.
    • It must be possible for more than one philosopher to eat at the same time.
    
    In this solution while Tanenbaum’s solution effectively avoids deadlock, it does not fully address starvation.
    Further enhancements are required to ensure that no philosopher is left waiting indefinitely.
    
    In Tanenbaum's solution, philosophers exist in three states: thinking, hungry, hungry, eating.
    The functions get_forks and put_forks allow each philosopher to eat or wait based on its state (EState).
    The test function checks whether each philosopher is allowed to eat based on the state of other philosophers around it. If the neighbors are not eating, the philosopher starts eating.
    This solution avoids deadlock, but starvation may not be completely resolved.
'''

philosopherAmount = 5
class State(Enum):
    THINKING = 1
    HUNGRY = 2
    EATING = 3

states = [State.THINKING for i in range(philosopherAmount)]
forks = [threading.Semaphore(0) for i in range(philosopherAmount)]
mutex = threading.Semaphore(1)

def left(_index): return _index
def right(_index): return (_index + 1) % 5
def think(): print('thinking...')
def eat(): print('eating...')
def get_forks(_index):
    mutex.acquire()
    states[_index] = State.HUNGRY
    test(_index)
    mutex.release()
    forks[_index].acquire()


def put_forks(_index):
    mutex.acquire()
    states[_index] = State.THINKING
    test(right(_index))
    test(left(_index))
    mutex.release()

# This function checks the conditions under which a philosopher can eat
# and, if appropriate, allows that philosopher to pick up a fork and eat.
# This ensures synchronization and prevents philosophers from entering
# deadlock or starvation states.
def test(_index):
    # If this philosopher's condition is hungry and
    # If the philosopher to your left (left) and right (right) are not eating:
    if states[_index] == State.HUNGRY and states[left(_index)] != State.EATING and states[right(_index)] != State.EATING:
        states[_index] = State.EATING
        # This philosopher is allowed to eat (forks semaphore released)
        forks[_index].release()

def execute(_index):
    while True:
        think()
        get_forks(_index)
        eat()
        put_forks(_index)


threads = [threading.Thread(target=execute, args=(i,)) for i in range(philosopherAmount)]

for thread in threads:
    thread.start()

for thread in threads:
    thread.join()
