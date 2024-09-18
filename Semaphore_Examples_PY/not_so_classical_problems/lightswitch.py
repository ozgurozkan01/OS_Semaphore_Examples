import threading

class Lightswitch:
    def __init__(self):
        self.counter = 0
        self.mutex = threading.Lock()

    def lock(self, room_empty):
        with self.mutex:
            self.counter += 1
            if self.counter == 1:
                room_empty.acquire()

    def unlock(self, room_empty):
        with self.mutex:
            self.counter -= 1
            if self.counter == 0:
                room_empty.release()
