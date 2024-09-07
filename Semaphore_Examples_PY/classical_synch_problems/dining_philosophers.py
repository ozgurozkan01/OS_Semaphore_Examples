import threading

'''
         - IT HAS JUST SOLUTION 1 -

         - CONSTRAINTS !!
            • Only one philosopher can hold a fork at a time.
            • It must be impossible for a deadlock to occur.
            • It must be impossible for a philosopher to starve waiting for a fork.
            • It must be possible for more than one philosopher to eat at the same time.
            
         This solution uses a footman, a “servant” or control mechanism. The footman counter initially has a value of 4,
         which means that a maximum of 4 philosophers can be at the table at the same time. This prevents 5 philosophers
         from requesting cutlery at the same time, which eliminates the possibility of deadlock.
'''

philosopherAmount = 5
forks = [threading.Semaphore(1) for i in range(philosopherAmount)]
footman = threading.Semaphore(4)

def left(_index) : return _index
def right(_index) : return (_index + 1) % 5
def think(): print('thinking...')
def eat(): print('eating...')

'''
    - NON SOLUTION !!    
    def get_forks(_index):
        forks[right(_index)].acquire()
        forks[left(_index)].acquire()
    
    def put_forks(_index):
        forks[right(_index)].release()
        forks[left(_index)].release()
    
    SO, WHY !! 
    The problem is that the table is round. As a result, each philosopher can pick
    up a fork and then wait forever for the other fork. that causes deadlock!
'''


def get_forks(_index):
    footman.acquire()
    forks[right(_index)].acquire()
    forks[left(_index)].acquire()


def put_forks(_index):
    forks[right(_index)].release()
    forks[left(_index)].release()
    footman.release()

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