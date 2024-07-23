//
// Created by ozgur on 7/23/2024.
//

#ifndef SEMAPHORE_EXAMPLES_CPP_INTRODUCTION_H
#define SEMAPHORE_EXAMPLES_CPP_INTRODUCTION_H

#include <iostream>
#include <condition_variable>

namespace introduction
{
    // SYNCHRONIZATION - means making two things happen at the same time

    /*
        !!! Solution
        To solve the syncronization problem in the 4 example above, we can use mutex or semaphore.
        Becayse mutex can lock the thread execution until it is unlocked.
    */

    namespace serialization
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
        std::mutex mtx;

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

    namespace non_determinism
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

    namespace concurrent_writes
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

    namespace concurrent_updates
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
}


#endif //SEMAPHORE_EXAMPLES_CPP_INTRODUCTION_H
