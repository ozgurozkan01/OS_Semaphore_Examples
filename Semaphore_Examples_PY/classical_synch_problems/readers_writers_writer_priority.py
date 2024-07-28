import threading

# -------------------------------------

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

         - ANALOGY !!
         Lightswitch, by analogy with the pattern where the first person into a room turns on the light (locks the mutex)
         and the last one out turns it off (unlocks the mutex).

         - WARNING ( STARVING PROBLEM )!!
         In this example, deadlock is not possible, but there is a related problem that is almost as bad: it is possible for a writer to starve.
         If a writer arrives while there are readers in the critical section, it might wait in queue forever while readers come and go.
         As long as a new reader arrives before the last of the current readers departs, there will always be at least one reader in the room.

         - PAY ATTENTION !!
         This situation is not a deadlock, because some threads are making progress, but it is not exactly desirable.

'''

readSwitch = Lightswitch()
writeSwitch = Lightswitch()
noReaders = threading.Semaphore(1)
noWriters = threading.Semaphore(1)

def writer_execute():
    writeSwitch.lock(noReaders)
    noWriters.acquire()
    print('Writer-Priority : Writer in critical section\n')
    noWriters.release()
    writeSwitch.unlock(noReaders)

def reader_execute():
    noReaders.acquire()
    readSwitch.lock(noWriters)
    noReaders.release()
    print('Writer-Priority : Reader in critical section')
    readSwitch.unlock(noWriters)


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






