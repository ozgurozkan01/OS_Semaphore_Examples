import queue
import threading
import time
import random

'''
    if items >= bufferSize :
        block ()
    
    This implementation does not work properly. Remember that we can’t check the current value of a semaphore
'''

bufferSize = 3
mutex = threading.Semaphore(1)
items = threading.Semaphore(0) # initial value is zero beacuse the buffer is empty in the beginning
buffer = queue.Queue()
# When a consumer removes an item it should signal spaces. When a producer
# arrives it should decrement spaces, at which point it might block until the next
# consumer signals.
spaces = threading.Semaphore(bufferSize)

class Event:
    def __init__(self, eventName):
        self.eventName = eventName
    def process(self):
        print(f"{self.eventName}. event is processsing")

def waitForEvent():
    randomInt = random.randint(1, 100)
    newEvent = Event(randomInt)
    time.sleep(2) # wait for 2 seconds
    return newEvent

def execute_producer():
    while True:
        event = waitForEvent()
        spaces.acquire()
        mutex.acquire()
        buffer.put(event)
        mutex.release()
        items.release()
        time.sleep(1) # custom code piece

def execute_consumer():
    while True:
        items.acquire()
        mutex.acquire()
        event = buffer.get()
        mutex.release()
        spaces.release()
        event.process()
        time.sleep(1) # custom code piece

producers = [threading.Thread(target=execute_producer) for _ in range(3)]
consumers = [threading.Thread(target=execute_consumer) for _ in range(3)]

for producer in producers:
    producer.start()

for consumer in consumers:
    consumer.start()

for producer in producers:
    producer.join()

for consumer in consumers:
    consumer.join()