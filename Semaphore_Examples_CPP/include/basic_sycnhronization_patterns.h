//
// Created by ozgur on 7/23/2024.
//

#ifndef SEMAPHORE_EXAMPLES_CPP_BASIC_SYCNHRONIZATION_PATTERNS_H
#define SEMAPHORE_EXAMPLES_CPP_BASIC_SYCNHRONIZATION_PATTERNS_H

#include <semaphore>
#include <barrier>
#include <iostream>
#include "Barrier.h"

namespace basic_synchronization_patterns
{
    namespace signaling
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

    namespace synch_dot_py
    {
        // NOT IMPLEMENTED YET
    }

    namespace rendezvous
    {
        /*
        Thread A            Thread B
        1 statement a1      1 statement b1
        2 statement a2      2 statement b2

        - CODE OUTPUT !!
        In this implementation both a1 and b1 will write as output before a2 and b2.
        We have to guaratee that. And it does not matter whether a1 or b1 is written first.

        - HINT IN BOOK TO IMPLEMENTATION !!

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

    namespace rendezvous_deadlock
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

         - CODE OUTPUT !!
         No fully output. Because of  no signaling. Both threads are writing the first statements but not writing the second one.
         Becuase both threads call the acquire() first but not get in and call release() because of the semaphores initial value is 0
         and that causes the deadlocks.
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

    namespace mutex
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

         - CODE OUTPUT !!
        This code gives output as 20.000.000 thanks to mutex. Because mutex ensures that the threads reach the value x with a synchronization.
        That means if a thread reaches the value, other threads wait until the mutex is available.
         That prevents reading and writing this x value at the same time, ensuring that the output is correct.
      */

        std::mutex mtx;
        int x = 0;

        void increase()
        {
            for (int i = 0; i < 10'000'000; ++i)
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

            if (tA.joinable()) { tA.join(); }
            if (tB.joinable()) { tB.join(); }

            std::cout << "Result : " << x << std::endl;

            return 0;
        }
    }

    namespace multiplex
    {
        /*

        MULTIPLEX : To allow multiple threads run in the critical section. To do that, we can use Semaphore which is value bigger than 1.

        In program, there are 5 threads but the semaphore has 4 as max value.
        When you run the program, 4 threads can run at the same time but
        the last one cannot get inside the semaphore. Because there is no room for it.

        We could not release the threads to show when semaphore is full threads cannot get inside.
        But at the same time, we saw that multiple threads can run at the same time

         - CODE OUTPUT !!
         The output of this code is the “. thread is running...” output that 4 threads will print,
         but the 5th thread cannot enter because there is no space in the multiplex and therefore cannot output.

         !! To run the correct program, put any thread in the comment line.
         */

        std::counting_semaphore<4> multiplex(4);

        void execute(int _no)
        {
            multiplex.acquire();
            std::cout << _no << ". thread is running...\n";
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

    namespace barrier_deadlock
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
       This causes the first n-1 thread to be blocked indefinitely in barrier.acquire()
       because barrier.release() is called only once or for one thread.

       To solve this problem, we need to make sure that all threads can continue after barrier.
       Solution I found is that when n == count, then barrier should be released for all threads.

    */

        const uint8_t n = 5;
        int count = 0;
        std::mutex mutex;
        std::counting_semaphore<n> barrier(0);
        // std::barrier barrier_alternative(0);

        void execute()
        {
            mutex.lock();
            count++;
            mutex.unlock();

            // BOOK IMPLEMENTATION
            // NOT WORKING
            if (count == n) { barrier.release();} // Increase the barrier value by 1, we have 5 threads
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

    namespace barrier_solution
    {
        /*
         - CODE LOGIC !!
         When n-1 thread reaches acquire(), none of them can enter, because the initial value of the barrier is already 0.
         The nth thread releases the barrier and the value of the barrier increases by 1.
         This increase allows the first thread that reaches acquire() to enter.
         In the next line, the thread calls release(), increases the barrier by 1 and
         the threads waiting at acquire() can enter respectively. Here, the value of the barrier is continuously decreasing and
         increasing by 1 0 1 0 1 0 1 0 and the thread that enters allows the next thread to enter.

         - CODE OUTPUT !!
         Output is all threads write "Barrier reached to the end!" as output.
         */

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

            std::cout << "Barrier reached to the end!" << std::endl;
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

    namespace barrier_deadlock_2
    {
        /*
        WHY IT IS NOT WORKING !!
        Becuase initial value of barrier is 0. So it is not acquirable. When thread1 arrived barrier.acquire(),
        it cannat pass through barrier. Because of that locked mutex never gonna be unlocked. And other threads
        never gonna get inside and wait forever.

         - CODE OUTPUT !!
         NO OUTPUT because of the deadlock.
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

            std::cout << "Barrier reached to the end!" << std::endl;
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

    namespace reusable_barrier_deadlock
    {
        /*
        Code seems to work, but unfortunately there are logic errors inside that can cause a synchronization error.

        Failure Scenario !!
           First Iteration: In the first iteration, threads might reach the barrier, increment count, and correctly proceed through the semaphore.
           Subsequent Iterations: In the next iteration, the state of the semaphore may not be reset correctly:
               If a thread acquires the semaphore after the turnstile.acquire() call, but before all threads have reached the barrier in the next iteration, it will proceed incorrectly.
               Other threads will be left waiting indefinitely because the semaphore count may not align with the required number of threads.


         - CODE OUTPUT !!
         The code can give output and this output is “Reusable barrier reached to the end!” as many as the number of threads.
         However, due to a logic error in the code, the code may enter deadlock. In this case there is no output.
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

            std::cout << "Reusable barrier reached to the end!" << std::endl;
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

    namespace reusable_barrier_deadlock_2
    {
        /*
         This code seems to write corrcet but there is some kind of synch problems.

         Potential Deadlock:
         The second turnstile.acquire() could cause a deadlock because if the last thread to decrement count
         is the same one that released the semaphore, no other thread will be able to release it again.

         - CODE OUTPUT !!
         The code can give output and this output is “Reusable barrier reached to the end!” as many as the number of threads.
         However, due to a logic error in the code, the code may enter deadlock. In this case there is no output.
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

            std::cout << "Reusable barrier reached to the end!" << std::endl;
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

    namespace reusable_barrier_solution
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

         - CODE OUTPUT !!
         The output is n pieces “Reusable barrier reached to the end!”.
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

            std::cout << "Reusable barrier reached to the end!" << std::endl;
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

    namespace preloaded_turnstile
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

       - CODE OUTPUT !!
         The output is n pieces “Preloaded barrier reached to the end!”.
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

            std::cout << "Preloaded barrier reached to the end!" << std::endl;
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


    namespace barrier_object
    {
        void run()
        {
            Barrier barrier(3); // initiliaze a new barrier
            barrier.wait();     // Waiting till forever - Deadlock
        }
    }

    namespace leader_and_follower_queue
    {
        /*
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

         */


        int leaders = 0;
        int followers = 0;

        std::mutex mutex;
        std::counting_semaphore<4> leaderQueue(0);
        std::counting_semaphore<4> followerQueue(0);
        std::counting_semaphore<4> rendezvous(0);

        void dance(std::string _dancer, int _dancerCode)
        {
            std::cout << _dancerCode << ". " << _dancer << " is dancing..." << std::endl;
        }

        void leaderExecute(int _leaderCode)
        {
            mutex.lock();
            if (followers > 0)
            {
                followers--;
                followerQueue.release();
            }
            else
            {
                leaders++;
                mutex.unlock();
                leaderQueue.acquire();
            }

            dance("leader", _leaderCode);
            rendezvous.acquire();
            mutex.unlock();
        }

        void followerExecute(int _followerCode)
        {
            mutex.lock();
            if (leaders > 0)
            {
                leaders--;
                leaderQueue.release();
            }
            else
            {
                followers++;
                mutex.unlock();
                followerQueue.acquire();
            }

            dance("follower", _followerCode);
            rendezvous.release();
        }

        void run()
        {
            std::thread leader1(leaderExecute, 1);
            std::thread leader2(leaderExecute, 2);
            std::thread leader3(leaderExecute, 3);

            std::thread follower1(followerExecute, 1);
            std::thread follower2(followerExecute, 2);
            std::thread follower3(followerExecute, 3);

            if (leader1.joinable()) { leader1.join(); }
            if (leader2.joinable()) { leader2.join(); }
            if (leader3.joinable()) { leader3.join(); }

            if (follower1.joinable()) { follower1.join(); }
            if (follower2.joinable()) { follower2.join(); }
            if (follower3.joinable()) { follower3.join(); }
        }
    }
}

#endif //SEMAPHORE_EXAMPLES_CPP_BASIC_SYCNHRONIZATION_PATTERNS_H
