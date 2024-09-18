import threading
import random
import time
from lightswitch import Lightswitch
from singly_linked_list import SinglyLinkedList

"""

        // To show properly how it is running, added single linked list to the problem.

            - DEFINITON OF PROBLEM !
                Three types of threads perform different tasks on a single linked list:
                searchers, inserters and deleters. Search threads only examine and
                read the contents of the list, and they can run concurrently with each
                other because their read operations do not modify the data, so they do not conflict
                with each other. Insertion threads add new items to the end of the list and these
                operations must be mutually exclusive, i.e. no two insertions can occur at the same
                time. However, insertion operations can be executed at the same time as search
                threads. Delete threads, on the other hand, remove items from the contents of the list
                and must be mutually exclusive with search and insert operations, i.e. neither search
                nor insert can be executed at the same time as a delete operation. Add and delete
                operations must be synchronized to maintain the consistency and data integrity of the
                list.

            - CODE OUTPUT !
                32 could not find in the list by searching...
                28 is inserted to the list...
                11 could not find in the list for deleting...
                41 is inserted to the list...35 could not find in the list by searching...
                
                36 could not find in the list for deleting...
                46 is inserted to the list...3 could not find in the list by searching...
                
                12 could not find in the list for deleting...
                20 could not find in the list by searching...
                27 is inserted to the list...
                7 could not find in the list for deleting...
                38 is inserted to the list...
                10 could not find in the list by searching...
                8 could not find in the list for deleting...
"""

# GLOBAL VARIABLES
insert_mutex = threading.Lock()
no_searcher = threading.Lock()
no_inserter = threading.Lock()
search_switch = Lightswitch()
insert_switch = Lightswitch()
test_list = SinglyLinkedList()

def execute_searcher():
    while True:
        search_switch.lock(no_searcher)
        # CRITICAL SECTION
        searching_value = random.randint(0, 50)
        is_found = test_list.search(searching_value)
        if is_found:
            print(f"{searching_value} exists in the list...")
        else:
            print(f"{searching_value} could not find in the list by searching...")
        search_switch.unlock(no_searcher)
        time.sleep(1)

def execute_inserter():
    while True:
        insert_switch.lock(no_inserter)
        with insert_mutex:
            # CRITICAL SECTION
            appending_value = random.randint(0, 50)
            test_list.append(appending_value)
            print(f"{appending_value} is inserted to the list...")
        insert_switch.unlock(no_inserter)
        time.sleep(1)

def execute_deleter():
    while True:
        no_searcher.acquire()
        no_inserter.acquire()
        # CRITICAL SECTION
        deleting_value = random.randint(0, 50)
        is_deleted = test_list.delete_value(deleting_value)
        if is_deleted:
            print(f"{deleting_value} is deleted from the list...")
        else:
            print(f"{deleting_value} could not find in the list for deleting...")
        no_inserter.release()
        no_searcher.release()
        time.sleep(1)


threads = [
    threading.Thread(target=execute_searcher),
    threading.Thread(target=execute_inserter),
    threading.Thread(target=execute_deleter)
]

for thread in threads:
    thread.start()

for thread in threads:
    thread.join()