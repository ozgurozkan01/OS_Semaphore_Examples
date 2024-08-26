//
// Created by ozgur on 7/23/2024.
//

#ifndef SEMAPHORE_EXAMPLES_CPP_CLASSICAL_SYNCHRONIZATION_PROBLEMS_H
#define SEMAPHORE_EXAMPLES_CPP_CLASSICAL_SYNCHRONIZATION_PROBLEMS_H

#include <thread>
#include <utility>
#include <queue>
#include <iostream>
#include <random>

#define WRITER_PRIORITY 1

namespace classical_synchronization_problems
{
    /*
        - What is the PRODUCER-CONSUMER Problem !!

        - WARNING !!
        This example too long, because of that, in this example, there is more than one example implementation.
        Pay attention to the comment sectins.

        - LOGIC OF RUNNING !!

        Producer :
        A producer thread continuously generates an event and adds it to the buffer. After this addition,
        the mutex (item) is unlocked and access to the critical section is allowed.
        The items.release() function called afterwards signals the consumer to execute the event.
        The consumer cannot execute the event until this signal is given.

        Consumer :
        For items.acquire() to work, the value must be greater than 0. After acquire(), if the critical section is empty,
        the thread enters and pops an event from the buffer to execute. After the pop, it exits the critical section and releases the mutex and executes the event.

        - CODE OUTPUT !!

        Example Output:
        76 is processed...
        96 is processed...
        92 is processed...
        12 is processed...
        8 is processed...
        63 is processed...

     */

    class Event
    {
    public:
        explicit Event(std::string _eventName = "noEvent") : eventName(std::move(_eventName)) {}
        void process() { std::cout << eventName << " is processed..." << std::endl; }
        std::string eventName;
    };

    namespace producer_consumer_problem_infinite
    {
        // Exclusive access to the buffer
        std::mutex mutex;
        // When items is positive, it indicates the number of items in the buffer.
        // When it is negative, it indicates the number of consumer threads in queue
        std::counting_semaphore<3> items(0);
        std::queue<Event> infiniteEventBuffer;

        Event waitForEvent()
        {
            // Random Number Generator : Better than rand() implementation
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> randomNumberGenerator(1, 100); // random int number between 1-100
            Event event(std::to_string(randomNumberGenerator(gen)));
            return event;
        }

/*
        // REGULAR SOLUTION
        void producer_execute()
        {
            // WaitForEvent
            Event event = waitForEvent(); // Local event
            mutex.lock();
            // buffer.add(event)
            infiniteEventBuffer.push(event);
            items.release();
            // items.release();
            mutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
*/

/*
        - WHY IT IS BETTER !!!
        Performance:
        By releasing the mutex before signaling the semaphore, you avoid holding the mutex while notifying consumers,
        reducing the chance of unnecessary contention. This can lead to better performance
        because consumers can acquire the mutex without waiting for the producer to finish its operations.

        Efficiency:
        It avoids the scenario where a consumer is blocked waiting for the semaphore while the mutex is held by the producer.
        This helps in reducing the waiting time for consumers and can lead to more efficient execution.
*/
        void improved_producer_execute()
        {
            while (true)
            {
                // WaitForEvent
                Event event = waitForEvent(); // Local event
                mutex.lock();
                // buffer.add(event)
                infiniteEventBuffer.push(event);
                // items.release();
                mutex.unlock();
                // If we use items.release() out off the mutex, that means improved producer solution
                items.release();
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            }
        }

/*
        - WHY IT IS BROKEN !!

        Blocking with Mutex Held:
        By acquiring the mutex before waiting on the semaphore (items.wait()),
        the consumer holds the mutex while it could potentially be blocked on the semaphore.
        This leads to inefficient use of the mutex and can cause other threads that need the mutex to be blocked unnecessarily.

        Reduced Efficiency:
        The holding of the mutex while waiting for an item to be available can lead to increased
        contention and potential delays, reducing the overall efficiency of the system.

        - BOOK EXPLANATION !!

        it can cause a deadlock. Imagine that the buffer is empty. A consumer arrives, gets the mutex, and then blocks on items.
        When the producer arrives, it blocks on mutex and the system comes to a grinding halt.
        This is a common error in synchronization code: any time you wait for a  semaphore while holding a mutex,
        there is a danger of deadlock. When you are checking a solution to a synchronization problem, you should check for this kind of deadlock.

        void broken_consumer_execute()
        {
            mutex.lock();
            items.acquire();
            Event event = infiniteEventBuffer.back(); // Local event
            mutex.unlock();
            event.process();
        }
*/

        void consumer_execute()
        {
            while (true)
            {
                items.acquire();
                mutex.lock();
                Event event = infiniteEventBuffer.front(); // Local event
                infiniteEventBuffer.pop();
                mutex.unlock();
                event.process();
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
            }
        }

        void run()
        {
            constexpr uint8_t maxElementNumber = 3;
            std::thread producers[maxElementNumber], consumers[maxElementNumber];

            for (int i = 0; i < maxElementNumber; ++i)
            {
                producers[i] = std::thread(improved_producer_execute);
                consumers[i] = std::thread(consumer_execute);
            }

            for (int i = 0; i < maxElementNumber; ++i)
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
         if (items >= bufferSize)
         {
            block();
         }

         This implementation does not work properly. Remember that we can’t check the current value of a semaphore

         - LOGIC OF RUNNING !!
            Compared to the previous example, this example is more fault tolerant and controlled because we control the buffer occupancy.

         - CODE OUTPUT !!
            Same output instance like in infinite producer-consumer problem
         */

        // Exclusive access to the buffer
        std::mutex mutex;
        // When items is positive, it indicates the number of items in the buffer.
        // When it is negative, it indicates the number of consumer threads in queue
        std::counting_semaphore<3> items(0);
        std::queue<Event> infiniteEventBuffer;
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
                infiniteEventBuffer.push(event);
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

                Event event = infiniteEventBuffer.front(); // Local event
                infiniteEventBuffer.pop();
                mutex.unlock();
                space.release(); // If we put in comment this line, when buffer is full, program gets in deadlock.

                event.process();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        void run()
        {
            constexpr uint8_t maxElementNumber = 3;
            std::thread producers[maxElementNumber], consumers[maxElementNumber];

            for (int i = 0; i < maxElementNumber; ++i)
            {
                producers[i] = std::thread(producer_execute);
                consumers[i] = std::thread(consumer_execute);
            }

            for (int i = 0; i < maxElementNumber; ++i)
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

    namespace dining_philosophers
    {
        #define IS_TANENBAUMS 0

        /*
         - IT HAS JUST SOLUTION 1 -

         - CONSTRAINTS !!
            • Only one philosopher can hold a fork at a time.
            • It must be impossible for a deadlock to occur.
            • It must be impossible for a philosopher to starve waiting for a fork.
            • It must be possible for more than one philosopher to eat at the same time.
         */


        struct fork
        {
            std::binary_semaphore sem;

            fork() : sem(0) {}
            explicit fork(int _value) : sem(_value) {}
        };

        constexpr int philosophersAmount = 5;

        // TODO
        // Create a list which named forks but this cannot be implemented
        // Because semaphores are non-moveable. it's about the lack of a copy or move constructor.
        // To solve this we can put it into a struct or a class.
        std::array<fork, philosophersAmount> forks {fork(1), fork(1), fork(1), fork(1), fork(1)};
        std::counting_semaphore<4> footman(4); // to solve deadlock

        int left(int _index) { return _index; } // Actually do not need it !
        int right(int _index) { return (_index + 1) % 5; }

#if IS_TANENBAUMS // If you want to use tanenbaums solution, activate the pre-processor variable !!

        namespace tanenbaums
        {
            /*
             In this solution while Tanenbaum’s solution effectively avoids deadlock, it does not fully address starvation.
             Further enhancements are required to ensure that no philosopher is left waiting indefinitely.
             */

            enum class EState : uint8_t
            {
                thinking,
                hungry,
                eating
            };

            std::array<EState, philosophersAmount> states = { EState::thinking, EState::thinking, EState::thinking, EState::thinking, EState::thinking};
            std::array<fork, philosophersAmount> tanenbaums_forks {fork(0), fork(0), fork(0), fork(0), fork(0)};
            std::mutex mutex;

            void test(int _index)
            {
                if (states[_index] == EState::hungry && states[left(_index)] != EState::eating && states[right(_index)] != EState::eating)
                {
                    states[_index] = EState::eating;
                    tanenbaums_forks[_index].sem.release();
                }
            }

            void put_forks(int _index)
            {
                mutex.lock();
                states[_index] = EState::thinking;
                test(right(_index));
                test(left(_index));
                mutex.unlock();
            }
            void get_forks(int _index)
            {
                mutex.lock();
                states[_index] = EState::hungry;
                test(_index);
                mutex.unlock();
                tanenbaums_forks[_index].sem.acquire();
            }
        }
#endif



        void think() { std::cout << std::this_thread::get_id() << " is thinking..." << std::endl; }
        void eat()   { std::cout << std::this_thread::get_id() << " is eating... yummy yummy..." << std::endl; }

        // Correct solution
        void get_forks(int _index)
        {
            #if !IS_TANENBAUMS
            footman.acquire();
            forks[right(_index)].sem.acquire();
            forks[left(_index)].sem.acquire();
            std::cout << std::this_thread::get_id() << " got fork..." << std::endl;
            #else
            tanenbaums::get_forks(_index);
            #endif
        }

        void put_forks(int _index)
        {
            #if !IS_TANENBAUMS
            forks[right(_index)].sem.release();
            forks[left(_index)].sem.release();
            footman.release();
            std::cout << std::this_thread::get_id() << " put fork..." << std::endl;
            #else
            tanenbaums::put_forks(_index);
            #endif
        }

        void execute(int _index)
        {
            while(true)
            {
                think();
                get_forks(_index);
                eat();
                put_forks(_index);
            }
        }

        void run()
        {
            std::vector<std::thread> threads;

            for (int i = 0; i < philosophersAmount; ++i)
            {
                threads.emplace_back(execute, i);
            }

            for (auto& thread : threads)
            {
                if (thread.joinable()) {
                    thread.join();
                }
            }
        }
    }

    namespace cigarette_smokers_deadlock
    {
        // THIS EXAMPLE FOCUSES ON INTERESTING VERSION IN "LittleBookOfSemaphores" !!

        /*
            PROBLEM OF THE SOLUTION - BOOK EXPLANATION

            At the beginning , This example has the possibility of deadlock.
            Imagine that the agent puts out tobacco and paper. Since the smoker with matches is waiting on tobacco,
            it might be unblocked. But the smoker with tobacco is waiting on paper,
            so it is possible (even likely) that it will also be unblocked.
            Then the first thread will block on paper and the second will block on match. Deadlock!

         */

        constexpr uint8_t agentsAmount = 3;
        constexpr uint8_t smokersAmount = 3;

        std::binary_semaphore agentSem(1);
        std::binary_semaphore tobacco(0);
        std::binary_semaphore paper(0);
        std::binary_semaphore match(0);

        void execute_agent(std::binary_semaphore& _ingredients1, std::binary_semaphore& _ingredients2)
        {
            while (1)
            {
                agentSem.acquire();
                _ingredients1.release();
                _ingredients2.release();
            }
        }

        void execute_smokers(std::binary_semaphore& _ingredients1, std::binary_semaphore& _ingredients2)
        {
            while (1)
            {
                _ingredients1.acquire();
                _ingredients2.acquire();
                agentSem.release();
            }
        }

        void run()
        {
            std::array<std::thread, agentsAmount> agents = {
                    std::thread(execute_agent, std::ref(tobacco), std::ref(paper)), // Agent A
                    std::thread(execute_agent, std::ref(paper), std::ref(match)), // Agent B
                    std::thread(execute_agent, std::ref(tobacco), std::ref(match))  // Agent C
            };

            std::array<std::thread, smokersAmount> smokers = {
                    std::thread(execute_smokers, std::ref(tobacco), std::ref(paper)), // Smoker with matches
                    std::thread(execute_smokers, std::ref(paper), std::ref(match)), // Smoker with tobacco
                    std::thread(execute_smokers, std::ref(tobacco), std::ref(match))  // Smoker with paper
            };

            for (auto& agent : agents)   { if (agent.joinable())  { agent.join();  } }
            for (auto& smoker : smokers) { if (smoker.joinable()) { smoker.join(); } }
        }
    }

    namespace cigarette_smokers_parnas_solution
    {
        // To solve deadlock
        // The boolean variables indicate whether or not an ingredient is on the table
        std::atomic<bool> isTobacco{false};
        std::atomic<bool> isPaper{false};
        std::atomic<bool> isMatch{false};

        // Beginning Code
        constexpr uint8_t agentsAmount = 3;
        constexpr uint8_t smokersAmount = 3;

        std::mutex mutex;

        std::binary_semaphore agentSem(1);
        std::binary_semaphore tobacco(0);
        std::binary_semaphore paper(0);
        std::binary_semaphore match(0);

        void execute_pusher(std::binary_semaphore& _mainIngredient, std::binary_semaphore& _ingredient1, std::binary_semaphore& _ingredient2,
                            std::atomic<bool>& _mainIndicator,      std::atomic<bool>& _indicator1,      std::atomic<bool>& _indicator2)
        {
            {
                _mainIngredient.acquire();
                mutex.lock();

                if (_indicator1)
                {
                    _indicator1 = false;
                    _ingredient2.release();
                }
                else if (_indicator2)
                {
                    _indicator2 = false;
                    _ingredient1.release();
                }
                else
                {
                    _mainIndicator = true;
                }

                mutex.unlock();
            }
        }

        void execute_agent(std::binary_semaphore& _ingredients1, std::binary_semaphore& _ingredients2)
        {
            while(true)
            {
                agentSem.acquire();
                _ingredients1.release();
                _ingredients2.release();
            }
        }

        void execute_smokers(std::binary_semaphore& _mainIngredients)
        {
            while(true)
            {
                _mainIngredients.acquire();
                // Make Cigaratte - no need to implement
                agentSem.release();
                // Smoke - no need to implement
            }
        }

        void run()
        {
            std::array<std::thread, agentsAmount> agents = {
                    std::thread(execute_agent, std::ref(tobacco), std::ref(paper)), // Agent A
                    std::thread(execute_agent, std::ref(paper), std::ref(match)), // Agent B
                    std::thread(execute_agent, std::ref(tobacco), std::ref(match))  // Agent C
            };

            std::array<std::thread, smokersAmount> smokers = {
                    std::thread(execute_smokers, std::ref(match)), // Smoker with matches
                    std::thread(execute_smokers, std::ref(tobacco)), // Smoker with tobacco
                    std::thread(execute_smokers, std::ref(paper))  // Smoker with paper
            };

            std::array<std::thread, agentsAmount> pushers = {
                    std::thread(execute_pusher, std::ref(tobacco), std::ref(paper),   std::ref(match), std::ref(isTobacco), std::ref(isPaper),   std::ref(isMatch)), // Pusher A
                    std::thread(execute_pusher, std::ref(paper),   std::ref(tobacco), std::ref(match), std::ref(isPaper),   std::ref(isTobacco), std::ref(isMatch)), // Pusher B
                    std::thread(execute_pusher, std::ref(match),   std::ref(tobacco), std::ref(paper), std::ref(isMatch),   std::ref(isTobacco), std::ref(isPaper))  // Pusher C
            };

            for (auto &agent: agents)   { if (agent.joinable())  { agent.join();  }}
            for (auto &smoker: smokers) { if (smoker.joinable()) { smoker.join(); }}
            for (auto &pusher: pushers) { if (pusher.joinable()) { pusher.join(); }}
        }
    }

    namespace cigarette_smokers_generalized_solution
    {
        // If the agents don’t wait for the smokers, ingredients might accumulate on the table.
        // Instead of using boolean values to keep track of ingredients, we need integers to count them.
        int numTobacco = 0;
        int numPaper = 0;
        int numMatch = 0;

        // Beginning Code
        constexpr uint8_t agentsAmount = 3;
        constexpr uint8_t smokersAmount = 3;

        std::mutex mutex;

        std::binary_semaphore agentSem(1);
        std::binary_semaphore tobacco(0);
        std::binary_semaphore paper(0);
        std::binary_semaphore match(0);

        void execute_pusher(std::binary_semaphore& _mainIngredient, std::binary_semaphore& _ingredient1, std::binary_semaphore& _ingredient2,
                            int& _mainIndicator, int& _indicator1, int& _indicator2)
        {
            {
                _mainIngredient.acquire();
                mutex.lock();

                if (_indicator1)
                {
                    _indicator1--;
                    _ingredient2.release();
                }
                else if (_indicator2)
                {
                    _indicator2--;
                    _ingredient1.release();
                }
                else
                {
                    _mainIndicator++;
                }

                mutex.unlock();
            }
        }

        void execute_agent(std::binary_semaphore& _ingredients1, std::binary_semaphore& _ingredients2)
        {
            while(true)
            {
                agentSem.acquire();
                _ingredients1.release();
                _ingredients2.release();
            }
        }

        void execute_smokers(std::binary_semaphore& _mainIngredients)
        {
            while(true)
            {
                _mainIngredients.acquire();
                // Make Cigaratte - no need to implement
                agentSem.release();
                // Smoke - no need to implement
            }
        }

        void run()
        {
            std::array<std::thread, agentsAmount> agents = {
                    std::thread(execute_agent, std::ref(tobacco), std::ref(paper)), // Agent A
                    std::thread(execute_agent, std::ref(paper), std::ref(match)), // Agent B
                    std::thread(execute_agent, std::ref(tobacco), std::ref(match))  // Agent C
            };

            std::array<std::thread, smokersAmount> smokers = {
                    std::thread(execute_smokers, std::ref(match)), // Smoker with matches
                    std::thread(execute_smokers, std::ref(tobacco)), // Smoker with tobacco
                    std::thread(execute_smokers, std::ref(paper))  // Smoker with paper
            };

            std::array<std::thread, agentsAmount> pushers = {
                    std::thread(execute_pusher, std::ref(tobacco), std::ref(paper),   std::ref(match), std::ref(numTobacco), std::ref(numPaper),   std::ref(numMatch)), // Pusher A
                    std::thread(execute_pusher, std::ref(paper),   std::ref(tobacco), std::ref(match), std::ref(numPaper),   std::ref(numTobacco), std::ref(numMatch)), // Pusher B
                    std::thread(execute_pusher, std::ref(match),   std::ref(tobacco), std::ref(paper), std::ref(numMatch),   std::ref(numTobacco), std::ref(numPaper))  // Pusher C
            };

            for (auto &agent: agents)   { if (agent.joinable())  { agent.join();  }}
            for (auto &smoker: smokers) { if (smoker.joinable()) { smoker.join(); }}
            for (auto &pusher: pushers) { if (pusher.joinable()) { pusher.join(); }}
        }
    }
}

#endif //SEMAPHORE_EXAMPLES_CPP_CLASSICAL_SYNCHRONIZATION_PROBLEMS_H
