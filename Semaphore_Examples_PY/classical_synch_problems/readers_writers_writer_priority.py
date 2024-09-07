import threading

"""
    All benchmarks and explanations are in readers_writers.py.
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






