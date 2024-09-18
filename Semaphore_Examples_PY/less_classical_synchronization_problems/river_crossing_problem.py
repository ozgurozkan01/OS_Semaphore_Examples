import threading

single_number = 4
pair_number = 2


barrier = threading.Barrier(4) # Barrier to synchronize after boarding the boat
mutex = threading.Lock() # Mutex for protecting shared variables
hacker_counter = 0 # Count of waiting hackers
serf_counter = 0 # Count of waiting serfs
hackers_fifo = threading.Semaphore(0) # Semaphore for hackers
serfs_fifo = threading.Semaphore(0) # Semaphore for serfs

def execute_hacker():
    global hacker_counter, serf_counter

    isCaptain = False
    with mutex:
        hacker_counter += 1
        if hacker_counter == single_number:
            # Allow all hackers to board the boat
            for _ in range(single_number):
                hackers_fifo.release()
            hacker_counter -= single_number
            isCaptain = True
        elif hacker_counter == pair_number and serf_counter >= pair_number:
            # Allow pair of hackers and serfs to board the boat
            for _ in range(pair_number):
                hackers_fifo.release()
                serfs_fifo.release()
            serf_counter -= pair_number
            hacker_counter -= pair_number
            isCaptain = True

    hackers_fifo.acquire()  # Wait until allowed to board
    print("Hacker : Board the boat")
    barrier.wait()  # Wait for others to finish boarding

    if isCaptain:
        print("Hacker : Captain rows the boat")
        # Release the mutex after captain rows the boat
        with mutex:
            pass

def execute_serf():
    global hacker_counter, serf_counter

    isCaptain = False
    with mutex:
        serf_counter += 1
        if serf_counter == single_number:
            # Allow all serfs to board the boat
            for _ in range(single_number):
                serfs_fifo.release()
            serf_counter -= single_number
            isCaptain = True
        elif serf_counter == pair_number and hacker_counter >= pair_number:
            # Allow pair of serfs and hackers to board the boat
            for _ in range(pair_number):
                serfs_fifo.release()
                hackers_fifo.release()
            serf_counter -= pair_number
            hacker_counter -= pair_number
            isCaptain = True

    serfs_fifo.acquire()  # Wait until allowed to board
    print("Serf : Board the boat")
    barrier.wait()  # Wait for others to finish boarding

    if isCaptain:
        print("Serf : Captain rows the boat")
        # Release the mutex after captain rows the boat
        with mutex:
            pass

def run():
    threads = []

    for _ in range(single_number):
        threads.append(threading.Thread(target=execute_hacker))
        threads.append(threading.Thread(target=execute_serf))

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()

if __name__ == "__main__":
    run()
