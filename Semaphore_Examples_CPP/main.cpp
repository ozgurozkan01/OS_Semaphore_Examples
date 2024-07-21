#include <iostream>
#include <mutex>
#include <semaphore>
#include <condition_variable>
#include <barrier>

std::mutex mtx;

namespace synchronization_1_Serialization
{
/*
    Serialization : Event A must happen before Event B

    Thread A (You)          Thread B (Bob)
    1 Eat breakfast         1 Eat breakfast
    2 Work                  2 Wait for a call
    3 Eat lunch             3 Eat lunchS
    4 Call Bob

    !!!!!
    It doesn't matter when Thread B has breakfast.
    He can do it before A or after. But he must always have lunch after A.

    Eeating breakfast concurrently but lunch sequentially
 */

    std::condition_variable cv;
    bool isBobCalled = false;

    void threadA()
    {
        std::cout << "Me: Eating breakfast" << std::endl;
        std::cout << "Me: Working" << std::endl;
        std::cout << "Me: Eating lunch" << std::endl;

        std::cout << "Me: Calling Bob" << std::endl;
        isBobCalled = true;

        /*
         If we do not use cv.notify_one, the code may get stuck in some cases

         Timing Variability: If Thread B starts execution after Thread A sets isBobCalled to true,
         Thread B may not need to wait. It will check the condition and continue unimpeded.

         CPU Timing: The operating system's thread scheduler may sometimes allow Thread A
         to finish setting isBobCalled before Thread B reaches the wait call.

         BUT IT IS NOT RELIABLE !!!
         */

        // This code piece sends signals to waiting threads.
        cv.notify_one();
    }

    void threadB()
    {
        std::cout << "Bob: Eating breakfast" << std::endl;

        std::unique_lock<std::mutex> lock(mtx);

        cv.wait(lock, []{ return isBobCalled; });

        std::cout << "Bob: Received call from Me" << std::endl;
        std::cout << "Bob: Eating lunch" << std::endl;

        mtx.unlock();
    }

    void run()
    {
        std::thread bob(&threadB);
        std::thread me(&threadA);

        if (me.joinable()) { me.join(); }
        if (bob.joinable()) { bob.join(); }
    }
}

namespace synchronization_2_NonDeterminism
{
    /*
         Concurrent Programs : Multiple threads of execution, all occuring at the same time.
         Concurrent programs are often non-deterministic. So, it is hard to see what will happen.

        Thread A            Thread B
        print " yes "       print " no "
     */

    void threadA() { std::cout << "YES!\n" << std::endl; }
    void threadB() { std::cout << "NO!\n" << std::endl; }

    void run()
    {
        std::thread t1(&threadB);
        std::thread t2(&threadA);

        if (t1.joinable()) { t1.join(); }
        if (t2.joinable()) { t2.join(); }
    }
}

namespace synchronization_3_ConcurrentWrites
{
    /*
    Most of the time, most variables in most threads are local,
    meaning that they belong to a single thread and no other threads can access them.
    As long as that’s true, there tend to be few synchronization problems,
    because threads just don’t interact

     But usually some variables are shared among two or more threads; this is one of the ways threads interact with each other.
     For example, one way to communicate information between threads is for one thread to read a value written by another thread

     Thread A           Thread B
     x = 5              x = 7
     print x
     */

    void threadA(int& x)
    {
        x = 5;
        std::cout << x << std::endl;
    }

    void threadB(int& x)
    {
        x = 7;
    }

    int run()
    {
        int x = 0;
        std::thread b(threadB, std::ref(x));
        std::thread a(threadA, std::ref(x));

        a.join();
        b.join();

        return 0;
    }
}

namespace synchronization_4_ConcurrentUpdates
{
    /*
     At first glance, it is not obvious that there is a synchronization problem here.
     There are only two execution paths, and they yield the same result.
     The problem is that these operations are translated into machine language before execution,
     and in machine language the update takes two steps, a read and a write.

     Signaling makes it possible to guarantee that a section of code in one thread
     will run before a section of code in another thread; in other words, it solves the
     serialization problem.

     Thread A               Thread B
     count = count + 1      1 count = count + 1

     So the problem is that here each thread is reading the same value. Because of that,
     the value is not going increased 1 instead of 2.

     !! Note : small values does not show this synchronization problem. Because of that, I use loop to use bigger values.
     */

    int x = 0;

    void increase()
    {
        for (int i = 0; i < 100000; ++i)
        {
            // If you uncomment mtx variable synchronization problem would be ended.
            // mtx.lock();
            x = x + 1;
            // mtx.unlock();
        }
    }

    int run() {

        std::thread tA(increase);
        std::thread tB(increase);

        tA.join();
        tB.join();

        std::cout << "Result : " << x << std::endl;

        return 0;
    }
}

namespace semaphore_definition
{
    // SEMAPHORES - is a data structure that is useful for solving a variety of synchronization problems.

    std::counting_semaphore<3> semaphore(0);       // A semaphore its max count 3, means 3 thread access at the same time.
    std::binary_semaphore binarySemaphore(0);      // A semaphore its max count 1, means just 1 thread can access.
    std::counting_semaphore<1> binarySemaphore_alternative(0);      // A semaphore its max count 1, means just 1 thread can access.

    void semaphoreImplementation()
    {
        // To use semaphore we need functions.
        semaphore.acquire(); // Wait (P operation)
        semaphore.release(); // Signal (V operation)
    }

    // Gives an error
/*
    semaphore.acquire(); // Wait (P operation)
    semaphore.release(); // Signal (V operation)
*/

}

namespace synchronizationPatterns_1_Signaling
{
    std::binary_semaphore binarySem(0); // A semaphore its max count 1, means just 1 thread can access.

    /*
     In this implementation, the output should be the :
            statement a1
            statement b1
     Because to be able to call the acquire() semaphore should have a positive value.
     But initialized with 0. Because of that, the acquire cannot be called.
     First semaphore should be released. So, the first statement is going to be the a1.
     After releasing, b1 can be written as an output. Until releasing, threadB will wait.
     */

    void threadB()
    {
        binarySem.acquire();
        std::cout << "statement b1" << std::endl;
    }
    void threadA()
    {
        std::cout << "statement a1" << std::endl;
        binarySem.release();
    }

    void run()
    {
        std::thread tA(threadA);
        std::thread tB(threadB);

        tA.join();
        tB.join();
    }
}


namespace synchronizationPatterns_2_SyncDotPy
{
    // NOT IMPLEMENTED YET
}

namespace synchronizationPatterns_3_Rendezvous
{
    /*
        Thread A            Thread B
        1 statement a1      1 statement b1
        2 statement a2      2 statement b2

        In this implementation both a1 and b1 will write as output before a2 and b2.
        We have to guaratee that. And it does not matter whether a1 or b1 is written first.

        !!! HINT IN BOOK TO IMPLEMENTATION

        The chances are good that you were able to figure out a solution, but if not, here is a hint.
        Create two semaphores, named "aArrived" and "bArrived", and initialize them both to zero.
        As the names suggest, aArrived indicates whether Thread A has arrived at the rendezvous, and bArrived likewise.
     */

    std::binary_semaphore aArrived(0); // A semaphore its max count 1, means just 1 thread can access.
    std::binary_semaphore bArrived(0); // A semaphore its max count 1, means just 1 thread can access.

    /*
        REMINDING !!!
        If current value of semaphore is 0, so thread cannot keep going. Value should be positive to be able to
        be used by thread.
     */

    /*
        -- LOGIC --
        First of all, all semaphores are not available initially.
        We got that semaphores have a value of 0.
     */
    void threadA()
    {
        std::cout << "statement a1" << std::endl;
        // semaphore value is increased. Now value is 1 and it means available.
        aArrived.release();
        // semaphore value is decreased. But initially value is 0. So decreasing not valid.
        // Because value cannot be negative. So if bArrived is not released by threadB, threadA cannot move.
        bArrived.acquire();
        std::cout << "statement a2" << std::endl;
    }

    void threadB()
    {
        std::cout << "statement b1" << std::endl;
        // semaphore value is increased. Now value is 1 and it means available.
        bArrived.release();
        // semaphore value is decreased. But initially value is 0. So decreasing not valid.
        // Because value cannot be negative. So if aArrived is not released by threadA, threadB cannot move.
        aArrived.acquire();
        std::cout << "statement b2" << std::endl;
    }

    void run()
    {
        std::thread tA(threadA);
        std::thread tB(threadB);

        if (tA.joinable()) { tA.join(); }
        if (tB.joinable()) { tB.join(); }
    }
}

namespace synchronizationPatterns_4_Rendezvous_Deadlock
{
    /*
        DEADLOCK
        This is when threads wait for each other and never continue. In this case the program must be killed.

                Thread A            Thread B
        1 statement a1              1 statement b1
        2 bArrived.wait()           2 aArrived.wait()
        3 aArrived.signal()         3 bArrived.signal()
        4 statement a2              4 statement b2

        In this example both threads wait each other forever because of no signaling.
     */

    std::binary_semaphore aArrived(0);
    std::binary_semaphore bArrived(0);

    void runThreadA()
    {
        std::cout << "statement a1" << std::endl;
        bArrived.acquire(); // Lock semaphore
        aArrived.release(); // Release semaphore
        std::cout << "statement a2" << std::endl;
    }

    void runThreadB()
    {
        std::cout << "statement b1" << std::endl;
        aArrived.acquire(); // Lock semaphore
        bArrived.release(); // Release semaphore
        std::cout << "statement b2" << std::endl;
    }

    void run()
    {
        // Asymmetric solution : each thread calls the different function.
        std::thread tA(runThreadA);
        std::thread tB(runThreadB);

        if (tA.joinable()) { tA.join(); }
        if (tB.joinable()) { tB.join(); }
    }
}

namespace synchronizationPatterns_5_Mutex
{
    /*
        - REMINDING !!
        The mutex guarantees that only one thread accesses the shared variable at a time.
        To examine the example you can go to syncronization examples. to try a different way,
        in this example I used "lock_guard" with mutex

        - LOCK_GUARD !!
        lock_guards are used as an alternative to forgetting to unlock.
        Thanks to these structures, the possibility of forgetting the "unlock" operation in your projects is eliminated.

        - DIFFERENCE BETWEEN LOCK_GUARD and MUTEX/SEMAPHORE !!
            * Critical Section Constraint : When u create a lock_guard, the critical section is the rest of the scope.
            But the semaphore or mutex provides u to unlock on the line you want.

        Thread A                    Thread B
        1 count = count + 1         1 count = count + 1
     */

    int x = 0;

    void increase()
    {
        for (int i = 0; i < 10000000; ++i)
        {
            // In C++, alternative mutex implementation.
            std::lock_guard<std::mutex> guard(mtx);
            x = x + 1;
        }
    }

    int run() {

        // Symmetric solution : each thread calls the same function.
        std::thread tA(increase);
        std::thread tB(increase);

        if (tA.joinable())  tA.join();
        if (tB.joinable())  tB.join();

        std::cout << "Result : " << x << std::endl;

        return 0;
    }
}

namespace synchronizationPatterns_6_Multiplex
{
    /*
        MULTIPLEX : To allow multiple threads run in the critical section. To do that, we can use Semaphore which is value bigger than 1.

        In program, there are 5 threads but the semaphore has 4 as max value.
        When you run the program, 4 threads can run at the same time but
        the last one cannot get inside the semaphore. Because there is no room for it.

        We could not release the threads to show when semaphore is full threads cannot get inside.
        But at the same time, we saw that multiple threads can run at the same time

        !! To run the correct program, put any thread in the comment line.

     */

    std::counting_semaphore<4> multiplex(4);

    void execute(int _no)
    {
        // multiplex.acquire();
        std::cout << _no << ". threads running...\n";
    }

    void run()
    {
        std::thread thread1(execute, 1);
        std::thread thread2(execute, 2);
        std::thread thread3(execute, 3);
        std::thread thread4(execute, 4);
        std::thread thread5(execute, 5);

        if (thread1.joinable()) { thread1.join(); }
        if (thread2.joinable()) { thread2.join(); }
        if (thread3.joinable()) { thread3.join(); }
        if (thread4.joinable()) { thread4.join(); }
        if (thread5.joinable()) { thread5.join(); }
    }
}

namespace synchronizationPatterns_7_Barrier_Deadlock
{
    /*
        - BARRIER DEFINITION !!
        A barrier is a synchronization mechanism that enforces a condition where a specified number of threads
        must all reach a particular point of execution (the "barrier") before any of them are allowed to proceed beyond that point.
        This ensures that all threads are synchronized at the barrier, making it useful for tasks that require collective agreement or coordination.

        - PROGRAM DEFINITON !!
        Let's consider the Rendezvous problem again. In this problem, we could not use more than 2 threads.
        To provide the synchronization no thread should execute critical point until after all threads have executed rendezvous.
        TO provide this, we are gonna use barrier. Barrier is gonna be locked (0 or negative) until all thread arrives to the barrier.
        When this condition is true, barrier will be unlocked (1 or more).

        1 n = the number of threads
        2 count = 0                 --> keeps track of how many threads have arrived
        3 mutex = Semaphore (1)
        4 barrier = Semaphore (0)

        WHY SOLUTION IS NOT CORRECT ?
        Specifically, all threads call barrier.acquire() after incrementing the count,
        but only the last thread (when count == n) calls barrier.release().
        This causes the first n-1 thread to be blocked indefinitely in barrier.acquire() because barrier.release() is called only once.

        To solve this problem, we nned to make sure that all threads can continue after barrier.
        Solution I found is that when n == count, then barrier should be released for all threads.

     */

    int n = 5;
    int count = 0;
    std::mutex mutex;
    std::binary_semaphore barrier(0);
    // std::barrier barrier_alternative(0);

    void execute()
    {
        mutex.lock();
        count++;
        mutex.unlock();

        // BOOK IMPLEMENTATION
        // NOT WORKING
        if (count == n) { barrier.release();} // Increase the barrier value by 1
        // WORKING - Alternative Solution
        //if (count == n) { barrier.release(n);}

        barrier.acquire();


        // Alternative Barrier Implementation with CPP barrier lib.
        //barrier_alternative.arrive_and_wait();
    }

    void run()
    {
        std::thread thread1(execute);
        std::thread thread2(execute);
        std::thread thread3(execute);
        std::thread thread4(execute);
        std::thread thread5(execute);

        if (thread1.joinable()) { thread1.join(); }
        if (thread2.joinable()) { thread2.join(); }
        if (thread3.joinable()) { thread3.join(); }
        if (thread4.joinable()) { thread4.join(); }
        if (thread5.joinable()) { thread5.join(); }
    }
}

namespace synchronizationPatterns_7_BarrierSolution
{
    int n = 5;
    int count = 0;
    std::mutex mutex;
    std::binary_semaphore barrier(0);
    // std::barrier barrier_alternative(n);

    void execute()
    {
        mutex.lock(); // Decrease the mutex value by 1
        count++;
        mutex.unlock(); // Increase the mutex value by 1

        // BOOK IMPLEMENTATION
        if (count == n) { barrier.release();}

        barrier.acquire(); // Decrease the barrier value by 1
        barrier.release(); // Increase the barrier value by 1

        // Alternative implementation with CPP barrier lib
        //barrier_alternative.arrive_and_wait();

        std::cout << "execute is stopped!" << std::endl;
    }

    void run()
    {
        std::thread thread1(execute);
        std::thread thread2(execute);
        std::thread thread3(execute);
        std::thread thread4(execute);
        std::thread thread5(execute);

        if (thread1.joinable()) { thread1.join(); }
        if (thread2.joinable()) { thread2.join(); }
        if (thread3.joinable()) { thread3.join(); }
        if (thread4.joinable()) { thread4.join(); }
        if (thread5.joinable()) { thread5.join(); }
    }
}

namespace synchronizationPatterns_8_Barrier_Deadlock_2
{
    /*
        WHY IT IS NOT WORKING !!
        Becuase initial value of barrier is 0. So it is not acquirable. When thread1 arrived barrier.acquire(),
        it cannat pass through barrier. Because of that locked mutex never gonna be unlocked. And other threads
        never gonna get inside and wait forever.
     */


    int n = 5;
    int count = 0;
    std::mutex mutex;
    std::binary_semaphore barrier(0);

    void execute()
    {
        mutex.lock(); // Decrease the mutex value by 1
        count++;

        if (count == n) { barrier.release(); }

        barrier.acquire();
        // std::cout << "Acquired" << std::endl;
        barrier.release();
        // std::cout << "Released" << std::endl;

        mutex.unlock(); // Increase the mutex value by 1
    }

    void run()
    {
        std::thread thread1(execute);
        std::thread thread2(execute);
        std::thread thread3(execute);
        std::thread thread4(execute);
        std::thread thread5(execute);

        if (thread1.joinable()) { thread1.join(); }
        if (thread2.joinable()) { thread2.join(); }
        if (thread3.joinable()) { thread3.join(); }
        if (thread4.joinable()) { thread4.join(); }
        if (thread5.joinable()) { thread5.join(); }
    }
}

namespace synchronizationPatterns_9_ReusableBarrier_Deadlock
{
    /*
     Code seems to work, but unfortunately there are logic errors inside that can cause a synchronization error.

     Failure Scenario !!
        First Iteration: In the first iteration, threads might reach the barrier, increment count, and correctly proceed through the semaphore.
        Subsequent Iterations: In the next iteration, the state of the semaphore may not be reset correctly:
            If a thread acquires the semaphore after the turnstile.acquire() call, but before all threads have reached the barrier in the next iteration, it will proceed incorrectly.
            Other threads will be left waiting indefinitely because the semaphore count may not align with the required number of threads.
    */

    int n = 5;
    int count = 0;
    std::mutex mutex;
    std::binary_semaphore turnstile(0);

    void execute()
    {
        mutex.lock();
        count++;
        mutex.unlock();

        if (count == n) { turnstile.release(); }

        turnstile.acquire();
        turnstile.release();

        mutex.lock();
        count--;
        mutex.unlock();

        if (count == 0) { turnstile.acquire(); }
    }

    void run()
    {
        std::thread thread1(execute);
        std::thread thread2(execute);
        std::thread thread3(execute);
        std::thread thread4(execute);
        std::thread thread5(execute);

        if (thread1.joinable()) { thread1.join(); }
        if (thread2.joinable()) { thread2.join(); }
        if (thread3.joinable()) { thread3.join(); }
        if (thread4.joinable()) { thread4.join(); }
        if (thread5.joinable()) { thread5.join(); }
    }
}

namespace synchronizationPatterns_10_ReusableBarrier_Deadlock_2
{
    /*
    This code seems to write corrcet but there is some kind of synch problems.

    Potential Deadlock:
    The second turnstile.acquire() could cause a deadlock because if the last thread to decrement count
    is the same one that released the semaphore, no other thread will be able to release it again.
    */

    int n = 5;
    int count = 0;
    std::mutex mutex;
    std::binary_semaphore turnstile(0);

    void execute()
    {
        mutex.lock();
        count++;
        if (count == n) { turnstile.release(); }
        mutex.unlock();

        turnstile.acquire();
        turnstile.release();

        mutex.lock();
        count--;
        if (count == 0) { turnstile.acquire(); }
        mutex.unlock();
    }

    void run()
    {
        std::thread thread1(execute);
        std::thread thread2(execute);
        std::thread thread3(execute);
        std::thread thread4(execute);
        std::thread thread5(execute);

        if (thread1.joinable()) { thread1.join(); }
        if (thread2.joinable()) { thread2.join(); }
        if (thread3.joinable()) { thread3.join(); }
        if (thread4.joinable()) { thread4.join(); }
        if (thread5.joinable()) { thread5.join(); }
    }
}

namespace synchronizationPatterns_11_ReusableBarrierSolution
{
    /*
        To solve deadlock problem, we use 2 turnstile semaphore.

        turnstile:
        This semaphore is used to synchronize threads at the first barrier point.
        It ensures that all threads wait until the last thread has reached the barrier before proceeding.
        It is initialized to 0, meaning threads will block on turnstile.acquire() until it is released by the last thread.

        turnstile2:
        This semaphore is used to manage a second phase of synchronization, essentially acting as a signal for the second barrier point.
        It is initialized to 1, so it starts in the "unlocked" state. This allows the first thread to proceed after all threads have passed the first barrier.
        It ensures that once all threads have reached the first barrier and proceeded, they wait for all threads to reach the second barrier before continuing.
     */

    int n = 5;
    int count = 0;
    std::mutex mutex;
    std::binary_semaphore turnstile(0);
    std::binary_semaphore turnstile2(1);

    void execute()
    {
        mutex.lock(); // For synchronization
        count++;
        if (count == n)
        {
            turnstile2.acquire(); // Lock the turnstile2
            turnstile.release(); // Unlock the tunrstile for first thread
        }
        mutex.unlock();

        turnstile.acquire(); // Blocking for first thread
        turnstile.release(); // When first thread released, then it releases the thread which comes after it.

        mutex.lock(); // For synchronization
        count--;
        if (count == 0)
        {
            turnstile.acquire(); // Lock the turnstile
            turnstile2.release(); // Unlock the tunrstile2 for first thread
        }
        mutex.unlock();

        turnstile2.acquire(); // Blocking for first thread
        turnstile2.release(); // When first thread released, then it releases the thread which comes after it.
    }

    void run()
    {
        std::thread thread1(execute);
        std::thread thread2(execute);
        std::thread thread3(execute);
        std::thread thread4(execute);
        std::thread thread5(execute);

        if (thread1.joinable()) { thread1.join(); }
        if (thread2.joinable()) { thread2.join(); }
        if (thread3.joinable()) { thread3.join(); }
        if (thread4.joinable()) { thread4.join(); }
        if (thread5.joinable()) { thread5.join(); }
    }
}

namespace synchronizationPatterns_12_PreloadedTurnstile
{
    /*

     - DEFINITION OF PRELOADED TURNSTILE !!
     A preloaded turnstile is a turnstile synchronization component that is initialized with
     a predefined number of signals before threads attempt to pass through it.
     This preloading mechanism allows a specified number of threads to pass through the turnstile simultaneously,
     reducing the overhead associated with sequential thread passage.

     - LOGIC OF RUNNING !!
     When the nth thread arrives, it preloads the first turnstile with one signal for each thread.
     When the nth thread passes the turnstile, it “takes the last token” and leaves the turnstile locked again.
     The same thing happens at the second turnstile, which is unlocked when the last thread goes through the mutex.

     */

    int n = 5;
    int count = 0;
    std::mutex mutex;
    std::binary_semaphore turnstile(0);
    std::binary_semaphore turnstile2(0);

    void execute()
    {
        mutex.lock(); // For synchronization
        count++;
        if (count == n) { turnstile.release(n); /* Unlock the tunrstile for first thread */ }
        mutex.unlock();

        turnstile.acquire(); // Blocking for threads

        mutex.lock(); // For synchronization
        count--;
        if (count == 0) { turnstile2.release(n); /* Unlock the tunrstile2 for first thread */ }
        mutex.unlock();

        turnstile2.acquire(); // Blocking for threads
    }

    void run()
    {
        std::thread thread1(execute);
        std::thread thread2(execute);
        std::thread thread3(execute);
        std::thread thread4(execute);
        std::thread thread5(execute);

        if (thread1.joinable()) { thread1.join(); }
        if (thread2.joinable()) { thread2.join(); }
        if (thread3.joinable()) { thread3.join(); }
        if (thread4.joinable()) { thread4.join(); }
        if (thread5.joinable()) { thread5.join(); }
    }
}

#include "Barrier.h"

namespace synchronizationPatterns_13_BarrierObject
{
    void run()
    {
        Barrier barrier(3); // initiliaze a new barrier
        barrier.wait();     // Waiting - Deadlock
    }
};

namespace synchronizationPatterns_14_LeaderAndFollower
{
    void run()
    {

    }
}

int main()
{
    // SYNCHRONIZATION - means making two things happen at the same time

    /*
        !!! Solution
        To solve the syncronization problem in the 4 example above, we can use mutex or semaphore.
        Becayse mutex can lock the thread execution until it is unlocked.
    */

    // synchronization_1_Serialization::run();
    // synchronization_2_NonDeterminism::run();
    // synchronization_3_ConcurrentWrites::run();
    // synchronization_4_ConcurrentUpdates::run();

    // synchronizationPatterns_1_Signaling::run();
    // synchronizationPatterns_3_Rendezvous::run();
    // synchronizationPatterns_4_Rendezvous_Deadlock::run();
    // synchronizationPatterns_5_Mutex::run();
    // synchronizationPatterns_6_Multiplex::run();
    // synchronizationPatterns_7_Barrier_Deadlock::run();
    // synchronizationPatterns_7_BarrierSolution::run();
    // synchronizationPatterns_8_Barrier_Deadlock_2::run();
    // synchronizationPatterns_9_ReusableBarrier_Deadlock::run();
    // synchronizationPatterns_10_ReusableBarrier_Deadlock_2::run();
    // synchronizationPatterns_11_ReusableBarrierSolution::run();
    synchronizationPatterns_12_PreloadedTurnstile::run();
    // synchronizationPatterns_13_BarrierObject::run();
    // synchronizationPatterns_14_LeaderAndFollower::run();

    return 0;
}