import threading

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
