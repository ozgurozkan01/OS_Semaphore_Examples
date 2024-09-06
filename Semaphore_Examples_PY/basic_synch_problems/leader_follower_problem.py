import threading

"""
         - LOGIC OF RUNNING !!
         This code enables multithreading using mutex and semaphores to provide synchronization and matching between leaders and followers.
         The goal of the code is to coordinate the pairing and dancing of leaders and followers and to ensure that both sides move in a synchronized manner.

         A leader checks if there are followers. If there are followers, the leader and follower match.
         If there are no followers, it is added to the leaders queue and waits. A follower checks if there are leaders.
         If there are leaders, the follower and leader match. If there is no leader, it is added to the followers queue and waits.

         Both sides are made to dance and synchronize. rendezvous semaphore allows leaders and followers to dance in synchronization and come together collectively.

         - CODE OUTPUT !!
         The output of this code is follower, leader , follower , leader respectively. However, it does not matter who the follower and leader are.
         The code guarantees that there is only one follower and one leader in the scene at the same time.
         In the output you can see that the number of consecutive followers and leaders changes according to the thread speed, but each time the follower and leader are consecutive.
"""

leaders = 0
followers = 0

mutex = threading.Semaphore(1)
leaderQueue = threading.Semaphore(0)
followerQueue = threading.Semaphore(0)
rendezvous = threading.Semaphore(0)


def dance(dancer):
    print(f'{dancer} is dancing...')


def follower_execute():
    global followers
    global leaders

    mutex.acquire()
    if leaders > 0:
        leaders -= 1
        leaderQueue.release()
    else:
        followers += 1
        mutex.release()
        followerQueue.acquire()
    dance('follower')
    rendezvous.release()


def leader_execute():
    global followers
    global leaders

    mutex.acquire()
    if followers > 0:
        followers -= 1
        followerQueue.release()
    else:
        leaders += 1
        mutex.release()
        leaderQueue.acquire()
    dance('leader')
    rendezvous.acquire()
    mutex.release()


leader1 = threading.Thread(target=leader_execute)
leader2 = threading.Thread(target=leader_execute)
leader3 = threading.Thread(target=leader_execute)

follower1 = threading.Thread(target=follower_execute)
follower2 = threading.Thread(target=follower_execute)
follower3 = threading.Thread(target=follower_execute)

leader1.start()
leader2.start()
leader3.start()
follower1.start()
follower2.start()
follower3.start()

leader1.join()
leader2.join()
leader3.join()
follower1.join()
follower2.join()
follower3.join()
