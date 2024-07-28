//
// Created by ozgur on 7/23/2024.
//

#ifndef SEMAPHORE_EXAMPLES_CPP_CLASSICAL_SYNCHRONIZATION_PROBLEMS_H
#define SEMAPHORE_EXAMPLES_CPP_CLASSICAL_SYNCHRONIZATION_PROBLEMS_H

#include <thread>
#include <utility>
#include <vector>
#include <iostream>
#include <condition_variable>
#include <chrono>
#include <random>

#define WRITER_PRIORITY 1

namespace classical_synchronization_problems
{
    /*
        - What is the PRODUCER-CONSUMER Problem !!

        - WARNING !!
        This example to long, because of that, in this example, there is more than one example implementation.
        Pay attention to the comment sectins.
     */

    class Event
    {
    public:
        explicit Event(std::string _eventName = "noEvent") : eventName(std::move(_eventName)) {}
        void process() { std::cout << eventName << " is processed..." << std::endl; }
    private:
        std::string eventName;
    };

    namespace producer_consumer_problem_infinite
    {
        // Exclusive access to the buffer
        std::mutex mutex;
        // When items is positive, it indicates the number of items in the buffer.
        // When it is negative, it indicates the number of consumer threads in queue
        std::counting_semaphore<3> items(0);
        std::vector<Event> infiniteEventBuffer;

        Event waitForEvent()
        {
            // Random Number Generator : Better than rand() implementation
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> randomNumberGenerator(1, 100); // random int number between 1-100
            Event event(std::to_string(randomNumberGenerator(gen)));
            return event;
        }

        void producer_execute()
        {
            while (true)
            {
                // WaitForEvent
                Event event = waitForEvent(); // Local event
                mutex.lock();
                // buffer.add(event)
                infiniteEventBuffer.push_back(event);
                // items.release();
                mutex.unlock();
                // If we use items.release() out off the mutex, that means improved producer solution
                items.release();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        void consumer_execute()
        {
            while (true)
            {
                items.acquire();
                mutex.lock();

                // Bad Consumer Solution - DEADLOCK PROBLEM
                // If we use items.acquire() in the mutex, that means broken consumer solution.That causes deadlock problem.
                // items.acquire();

                Event event = infiniteEventBuffer.back(); // Local event
                infiniteEventBuffer.pop_back();
                mutex.unlock();
                event.process();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        void run()
        {
            constexpr uint8_t maxElementNumver = 3;
            std::thread producers[maxElementNumver], consumers[maxElementNumver];

            for (int i = 0; i < maxElementNumver; ++i)
            {
                producers[i] = std::thread(producer_execute);
                consumers[i] = std::thread(consumer_execute);
            }

            for (int i = 0; i < maxElementNumver; ++i)
            {
                producers[i].join();
                consumers[i].join();
            }
        }
    }

    namespace producer_consumer_problem_finite
    {
        /*
         - Broken Finite Buffer Solution !!
         if items >= bufferSize :
            block ()

         This implementation does not work properly. Remember that we can’t check the current value of a semaphore
         */

        // Exclusive access to the buffer
        std::mutex mutex;
        // When items is positive, it indicates the number of items in the buffer.
        // When it is negative, it indicates the number of consumer threads in queue
        std::counting_semaphore<3> items(0);
        std::vector<Event> infiniteEventBuffer;
        const int bufferSize = 3;
        std::counting_semaphore<bufferSize> space(bufferSize);

        Event waitForEvent()
        {
            // Random Number Generator : Better than rand() implementation
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> randomNumberGenerator(1, 100); // random int number between 1-100
            Event event(std::to_string(randomNumberGenerator(gen)));
            return event;
        }

        void producer_execute()
        {
            while (true)
            {
                // WaitForEvent
                Event event = waitForEvent(); // Local event
                space.acquire();
                mutex.lock();
                // buffer.add(event)
                infiniteEventBuffer.push_back(event);
                std::cout << "Buffer Size : " << infiniteEventBuffer.size() << std::endl;
                // items.release();
                mutex.unlock();
                // If we use items.release() out off the mutex, that means improved producer solution
                items.release();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        void consumer_execute()
        {
            while (true)
            {
                items.acquire();
                mutex.lock();

                // Bad Consumer Solution - DEADLOCK PROBLEM
                // If we use items.acquire() in the mutex, that means broken consumer solution.That causes deadlock problem.
                // items.acquire();

                Event event = infiniteEventBuffer.back(); // Local event
                infiniteEventBuffer.pop_back();
                mutex.unlock();
                space.release(); // If we put in comment this line, when buffer is full, program gets in deadlock.

                event.process();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        void run()
        {
            constexpr uint8_t maxElementNumver = 3;
            std::thread producers[maxElementNumver], consumers[maxElementNumver];

            for (int i = 0; i < maxElementNumver; ++i)
            {
                producers[i] = std::thread(producer_execute);
                consumers[i] = std::thread(consumer_execute);
            }

            for (int i = 0; i < maxElementNumver; ++i)
            {
                producers[i].join();
                consumers[i].join();
            }
        }
    }

    namespace readers_and_writers_problem
    {
        /*
         - ANALOGY !!
         Lightswitch, by analogy with the pattern where the first person into a room turns on the light (locks the mutex)
         and the last one out turns it off (unlocks the mutex).

         - WARNING ( STARVING PROBLEM )!!
         In this example, deadlock is not possible, but there is a related problem that is almost as bad: it is possible for a writer to starve.
         If a writer arrives while there are readers in the critical section, it might wait in queue forever while readers come and go.
         As long as a new reader arrives before the last of the current readers departs, there will always be at least one reader in the room.

         - PAY ATTENTION !!
         This situation is not a deadlock, because some threads are making progress, but it is not exactly desirable.

         - WRITER-PRIORITY SOLUTION !!

         */

        class Lightswitch
        {
        public:
            Lightswitch() : counter(0) {}

            void lock(std::mutex& _roomEmpty)
            {
                mutex.lock();
                counter++;
                if (counter == 1)
                {
                    _roomEmpty.lock();
                }
                mutex.unlock();
            }

            void unlock(std::mutex& _roomEmpty)
            {
                mutex.lock();
                counter--;
                if (counter == 0)
                {
                    _roomEmpty.unlock();
                }
                mutex.unlock();
            }
        private:
            int counter; // keeps track of how many readers are in the room
            std::mutex mutex;
        };

        // Code Base
        Lightswitch readLightSwitch;
        std::mutex roomEmpty; // 1 if there are no threads (readers or writers) in the critical section, and 0 otherwise
        // Starving Problem
        std::mutex turnstile; // To solve starving, turnstile for readers and a mutex for writers.
        // Writer-Priority
        #if WRITER_PRIORITY
        Lightswitch readSwitch;
        Lightswitch writeSwitch;
        std::mutex noReaders;
        std::mutex noWriters;
        #endif

        void writer_execute()
        {
        #if WRITER_PRIORITY
            writeSwitch.lock(noReaders);
            noWriters.lock();
                std::cout << "Writer-Priority : Writer in critical section" << std::endl;
            noWriters.unlock();
            writeSwitch.unlock(noReaders);
        #else
            turnstile.lock(); // To solve starving

            roomEmpty.lock();
                std::cout << "Writer in critical section" << std::endl;
            turnstile.unlock(); // To solve starving

            roomEmpty.unlock();
        #endif
        }

        void reader_execute()
        {
        #if WRITER_PRIORITY
            noReaders.lock();
            readSwitch.lock(noWriters);
            noReaders.unlock();
                std::cout << "Writer-Priority : Reader in critical section" << std::endl;
            readSwitch.unlock(noWriters);
        #else
            turnstile.lock(); // To solve starving
            turnstile.unlock(); // To solve starving

            readLightSwitch.lock(roomEmpty);
                std::cout << "Reader in critical section" << std::endl;
            readLightSwitch.unlock(roomEmpty);
        #endif
        }

        void run()
        {
            std::thread writer1(writer_execute);
            std::thread reader1(reader_execute);

            std::thread writer2(writer_execute);
            std::thread reader2(reader_execute);

            if (writer1.joinable()) { writer1.join(); }
            if (writer2.joinable()) { writer2.join(); }
            if (reader1.joinable()) { reader1.join(); }
            if (reader2.joinable()) { reader2.join(); }
        }
    }

    namespace no_starving_mutex
    {
        /*
         - WHY MORRIS'S ALGORITHM IS USED !!
            Starvation can occur when one or more threads are perpetually denied
            access to a resource because other threads continually consume the resource.

            Morris’s algorithm helps prevent starvation by ensuring that all threads will eventually gain
            access to the critical section, even if there are more threads trying to enter concurrently

          - WHY THERE ARE 2 PHASES !!
            Phase 1 ensures that threads are managed fairly before they enter the critical section.
            By using t1 and t2, the algorithm controls access to the critical section,
            ensuring only one thread executes the critical section at a time.
         */

        std::mutex mutex;
        // waiting threads amount
        int room1 = 0;
        int room2 = 0;
        // provides to control the flow of threads
        std::binary_semaphore t1(1); // available
        std::binary_semaphore t2(0); // unavailable

        void morris_algorithm()
        {
            while (true)
            {
                // phase 1
                mutex.lock();
                ++room1;
                mutex.unlock();

                t1.acquire();
                    ++room2;
                    mutex.lock();
                    --room1;

                    if (room1 == 0)
                    {
                        mutex.unlock();
                        t2.release();
                    }
                    else
                    {
                        mutex.unlock();
                        t1.release();
                    }

                // phase 2
                t2.acquire();
                    --room2;

                    std::cout << "Thread with " << std::this_thread::get_id() << "ids in Critical Section! "  << std::endl;

                    if (room2 == 0) { t1.release(); }
                    else            { t2.release(); }
            }
        }

        void run()
        {
            std::thread thread1(morris_algorithm);
            std::thread thread2(morris_algorithm);
            std::thread thread3(morris_algorithm);

            if (thread1.joinable()) { thread1.join(); }
            if (thread2.joinable()) { thread2.join(); }
            if (thread3.joinable()) { thread3.join(); }
        }
    }
}

#endif //SEMAPHORE_EXAMPLES_CPP_CLASSICAL_SYNCHRONIZATION_PROBLEMS_H
