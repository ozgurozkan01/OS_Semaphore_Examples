# Global Interpreter Lock (GIL): Python uses a GIL which ensures that only one thread
# executes Python bytecode at a time. This serialization inherently prevents multiple
# threads from concurrently accessing or modifying Python objects like integers (count
# in our case).

# In this code there is actually a sync problem, But GIL in Python prevents this.

import threading

counter = 0

def increment():
    global counter
    counter += 1

thread1 = threading.Thread(target=increment)
thread2 = threading.Thread(target=increment)

thread1.start()
thread2.start()

thread1.join()
thread2.join()

print(counter)
# The output is unpredictable
# This can print either 1 or 2