import threading

"""

         - LOGIC OF RUNNING !!
            There are two kinds of threads, oxygen and hydrogen. In order to assemble
            these threads into water molecules, we have to create a barrier that makes each
            thread wait until a complete molecule is ready to proceed.
            As each thread passes the barrier, it should invoke bond. You must guarantee
            that all the threads from one molecule invoke bond before any of the threads
            from the next molecule do.

            • If an oxygen thread arrives at the barrier when no hydrogen threads are
            present, it has to wait for two hydrogen threads.
            • If a hydrogen thread arrives at the barrier when no other threads are
            present, it has to wait for an oxygen thread and another hydrogen thread.

         - CODE OUTPUT !!
            2 atoms bound each other!2 atoms bound each other!
            2 atoms bound each other!2 atoms bound each other!
            
            2 atoms bound each other!
            2 atoms bound each other!2 atoms bound each other!2 atoms bound each other!
            
            
            
            2 atoms bound each other!
            2 atoms bound each other!2 atoms bound each other!2 atoms bound each other!
            
            2 atoms bound each other!2 atoms bound each other!
            2 atoms bound each other!
"""

mutex = threading.Lock() # Mutex for protecting shared variables
oxygen_counter = 0 # Count of waiting oxygen threads
hydrogen_counter = 0 # Count of waiting hydrogen threads
barrier = threading.Barrier(3) # Barrier to synchronize after bonding
oxygen_fifo = threading.Semaphore(0) # Semaphore for oxygen threads
hydrogen_fifo = threading.Semaphore(0) # Semaphore for hydrogen threads

def execute_oxygen():
    global oxygen_counter, hydrogen_counter

    with mutex:
        oxygen_counter += 1
        if hydrogen_counter >= 2:
            # Allow two hydrogen threads to bond
            for _ in range(2):
                hydrogen_fifo.release()
            hydrogen_counter -= 2
            # Allow one oxygen thread to bond
            oxygen_fifo.release()
            oxygen_counter -= 1
        else:
            # Release the mutex if no bonding can be done
            pass

    # Wait until allowed to bond
    oxygen_fifo.acquire()
    print("2 atoms bound each other!")
    # Wait for other threads to finish bonding
    barrier.wait()


def execute_hydrogen():
    global hydrogen_counter, oxygen_counter

    with mutex:
        hydrogen_counter += 1
        if hydrogen_counter >= 2 and oxygen_counter >= 1:
            # Allow two hydrogen threads to bond
            for _ in range(2):
                hydrogen_fifo.release()
            hydrogen_counter -= 2
            # Allow one oxygen thread to bond
            oxygen_fifo.release()
            oxygen_counter -= 1
        else:
            # Release the mutex if no bonding can be done
            pass

    # Wait until allowed to bond
    hydrogen_fifo.acquire()
    print("2 atoms bound each other!")
    # Wait for other threads to finish bonding
    barrier.wait()

threads = []

for _ in range(10):
    threads.append(threading.Thread(target=execute_hydrogen))

for _ in range(5):
    threads.append(threading.Thread(target=execute_oxygen))

for thread in threads:
    thread.start()

for thread in threads:
    thread.join()