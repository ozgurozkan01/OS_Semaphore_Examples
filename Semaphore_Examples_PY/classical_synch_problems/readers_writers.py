import threading

# -------------------------------------

"""
    - ANALOGY !!
    Lightswitch, by analogy with the pattern where the first person into a room turns on the light (locks the mutex)
    and the last one out turns it off (unlocks the mutex).

    The Lightswitch class manages access to a critical section using a semaphore and a counter.
    The lock method ensures that only the first reader will acquire the semaphore, and the last reader to leave will release it.
    The unlock method releases the semaphore if there are no more readers.
"""
class Lightswitch:
    def __init__(self):
        self.counter = 0
        self.mutex = threading.Semaphore(1)

    def lock(self, semaphore):
        self.mutex.acquire()
        self.counter += 1
        if self.counter == 1:
            semaphore.acquire()
        self.mutex.release()

    def unlock(self, semaphore):
        self.mutex.acquire()
        self.counter -= 1
        if self.counter == 0:
            semaphore.release()
        self.mutex.release()

# -------------------------------------

'''
         - WARNING ( STARVING PROBLEM )!!
         In this example, deadlock is not possible, but there is a related problem that is almost as bad: it is possible for a writer to starve.
         If a writer arrives while there are readers in the critical section, it might wait in queue forever while readers come and go.
         As long as a new reader arrives before the last of the current readers departs, there will always be at least one reader in the room.

         - PAY ATTENTION !!
         This situation is not a deadlock, because some threads are making progress, but it is not exactly desirable.

         - Writer Priority vs. No Writer Priority !!
         In Writer Priority, writers are granted access before readers. This is achieved through the writeSwitch and noReaders mutexes,
         so that a writer waits for all readers to exit before entering the critical region.
         This approach provides fair access to writers while limiting access to readers, thus minimizing the risk of starvation of writers.
         However, this may result in readers occupying the critical zone for too long, increasing the risk of writers being denied opportunities.

         In No Writer Priority, on the other hand, writers and readers are treated equally.
         turnstile mutex is used to ensure a fair queue between threads of both types.
         This approach harbors a risk of scarcity, where readers can continuously occupy the critical region
         because it gives no special priority to writers. If authors lose access opportunities, this can lead to scarcity problems.
         Therefore, this method can create inequity in providing access to authors and cause performance problems.

         - CODE OUTPUT !!
         In both cases the code output is very similar. However, due to race conditions, if writers have priority,
         it may be more likely that the writer is written first in the first output of the code.
         In simultaneous writer and reader conditions, writer is written first.
         Otherwise, it is difficult to observe a noticeable difference between the outputs.
         The output may be the same in both cases.

            Example Code of Writer-Priority
                Writer-Priority : Reader in critical section
                Writer-Priority : Writer in critical section
                Writer-Priority : Writer in critical section
                Writer-Priority : Reader in critical section

            Example Code of No-Writer-Priority
                No Writer-Priority : Writer in critical section
                No Writer-Priority : Reader in critical section
                No Writer-Priority : Writer in critical section
                No Writer-Priority : Reader in critical section
'''

readLightswitch = Lightswitch()
roomEmpty = threading.Semaphore(1)
turnstile = threading.Semaphore(1)

# WRITER PRIORITY VARIABLES
readSwitch = Lightswitch()
writeSwitch = Lightswitch()
noReaders = threading.Semaphore(1)
noWriters = threading.Semaphore(1)

def writer_execute():
    turnstile.acquire()

    roomEmpty.acquire()
    print('Writer in critical section')
    turnstile.release()

    roomEmpty.release()

def reader_execute():
    turnstile.acquire()
    turnstile.release()

    readLightswitch.lock(roomEmpty)
    print('Reader in critical section!\n')
    readLightswitch.unlock(roomEmpty)

writer1 = threading.Thread(target=writer_execute)
writer2 = threading.Thread(target=writer_execute)

reader1 = threading.Thread(target=reader_execute)
reader2 = threading.Thread(target=reader_execute)

reader1.start()
reader2.start()
writer1.start()
writer2.start()

reader1.join()
reader2.join()
writer1.join()
writer2.join()