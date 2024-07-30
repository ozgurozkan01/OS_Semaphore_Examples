import threading
from enum import Enum

'''
    In this solution while Tanenbaum’s solution effectively avoids deadlock, it does not fully address starvation.
    Further enhancements are required to ensure that no philosopher is left waiting indefinitely.
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

def test(_index):
    if states[_index] == State.HUNGRY and states[left(_index)] != State.EATING and states[right(_index)] != State.EATING:
        states[_index] = State.EATING
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
