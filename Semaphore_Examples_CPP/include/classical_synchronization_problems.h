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
#include <array>

namespace classical_synchronization_problems
{
    /*
        - WARNING !!
        This example too long, because of that, in this example, there is more than one example implementation.
        Pay attention to the comment sectins.
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
        /*

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
         - WARNING ( STARVING PROBLEM )!!
         In this example, deadlock is not possible, but there is a related problem that is almost as bad: it is possible for a writer to starve.
         If a writer arrives while there are readers in the critical section, it might wait in queue forever while readers come and go.
         As long as a new reader arrives before the last of the current readers departs, there will always be at least one reader in the room.

         - PAY ATTENTION !!
         This situation is not a deadlock, because some threads are making progress, but it is not exactly desirable.

         - Writer Priority vs. No Writer Priority !!
         In Writer Priority, writers are granted access before readers. This is achieved through the writeSwitch and noReaders mutexes,
         so that a writer waits for all readers to exit before entering the critical region.
         This approach provides fair access to writers while limiting access to readers, thus minimizing the risk of starvation of writers.
         However, this may result in readers occupying the critical zone for too long, increasing the risk of writers being denied opportunities.

         In No Writer Priority, on the other hand, writers and readers are treated equally.
         turnstile mutex is used to ensure a fair queue between threads of both types.
         This approach harbors a risk of scarcity, where readers can continuously occupy the critical region
         because it gives no special priority to writers. If authors lose access opportunities, this can lead to scarcity problems.
         Therefore, this method can create inequity in providing access to authors and cause performance problems.

         - CODE OUTPUT !!
         In both cases the code output is very similar. However, due to race conditions, if writers have priority,
         it may be more likely that the writer is written first in the first output of the code.
         In simultaneous writer and reader conditions, writer is written first.
         Otherwise, it is difficult to observe a noticeable difference between the outputs.
         The output may be the same in both cases.

            Example Code of Writer-Priority
                Writer-Priority : Reader in critical section
                Writer-Priority : Writer in critical section
                Writer-Priority : Writer in critical section
                Writer-Priority : Reader in critical section

            Example Code of No-Writer-Priority
                No Writer-Priority : Writer in critical section
                No Writer-Priority : Reader in critical section
                No Writer-Priority : Writer in critical section
                No Writer-Priority : Reader in critical section
         */

        class Lightswitch
        {
            /*
             - ANALOGY !!
            Lightswitch, by analogy with the pattern where the first person into a room turns on the light (locks the mutex)
            and the last one out turns it off (unlocks the mutex).

             The Lightswitch class manages access to a critical section using a semaphore and a counter.
             The lock method ensures that only the first reader will acquire the semaphore, and the last reader to leave will release it.
             The unlock method releases the semaphore if there are no more readers.
             */
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

        #define WRITER_PRIORITY 0

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
                std::cout << "No Writer-Priority : Writer in critical section" << std::endl;
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
                std::cout << "No Writer-Priority : Reader in critical section" << std::endl;
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

    namespace no_starve_mutex
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

                Phase 1: Each thread first increments the room1 counter and then waits for the t1 semaphore.
                This allows the thread to synchronize with other threads.
                If room1 is zero, this indicates that the thread is the first thread, so the t2 semaphore is released.
                The other threads release the t1 semaphore and go back to waiting.

                Phase 2: After waiting for semaphore t2, the thread accesses the critical region.
                After the critical zone operation, the thread decrements the room2 counter and if room2 is zero,
                it allows other threads to wait for the t1 semaphore.
                Otherwise, the t2 semaphore is released and other threads access the critical region.

            - CODE OUTPUT !!
            1. Phase - Room Management and Access Control:
                The first phase of the code ensures that each thread gets a fair turn before entering the
                critical area. In this phase, a thread indicates that it is in the waiting room by incrementing
                the counter named “room1” by one. Then, the thread waits to obtain the t1 semaphore.
                The semaphore determines the first thread that can enter the critical region.

                If room1 is zero (i.e. there are no other waiting threads), this thread releases semaphore t2
                and can enter the critical region. Other threads release semaphore t1 and wait for their
                turn to enter the critical region.

            2. Phase - Critical Zone Access and Exit:
                In the second phase, the thread waits for the t2 semaphore before entering the critical
                region. After accessing the critical region, the thread decrements the “room2” counter
                value by one and executes its operations in the critical region.

                When exiting the critical zone, if room2 has reached zero (i.e. there are no other threads
                waiting), it releases semaphore t1 and allows other threads to wait in phase one.
                Otherwise, it releases the t2 semaphore, allowing other threads to access the critical
                region.
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
         - CONSTRAINTS !!
            • Only one philosopher can hold a fork at a time.
            • It must be impossible for a deadlock to occur.
            • It must be impossible for a philosopher to starve waiting for a fork.
            • It must be possible for more than one philosopher to eat at the same time.
         - 2 SOLUTIONS !!
            Regular Solution
                This solution uses a footman, a “servant” or control mechanism. The footman counter initially has a value of 4,
                which means that a maximum of 4 philosophers can be at the table at the same time. This prevents 5 philosophers
                from requesting cutlery at the same time, which eliminates the possibility of deadlock.
            Tanenbaums Solution
                In Tanenbaum's solution, philosophers exist in three states: thinking, hungry, hungry, eating.
                The functions get_forks and put_forks allow each philosopher to eat or wait based on its state (EState).
                The test function checks whether each philosopher is allowed to eat based on the state of other philosophers around it. If the neighbors are not eating, the philosopher starts eating.
                This solution avoids deadlock, but starvation may not be completely resolved.
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

            /*
                This function checks the conditions under which a philosopher can eat
                and, if appropriate, allows that philosopher to pick up a fork and eat.
                This ensures synchronization and prevents philosophers from entering
                deadlock or starvation states.
             */
            void test(int _index)
            {
                // If this philosopher's condition is hungry and
                // If the philosopher to your left (left) and right (right) are not eating:
                if (states[_index] == EState::hungry && states[left(_index)] != EState::eating && states[right(_index)] != EState::eating)
                {
                    states[_index] = EState::eating;
                    // This philosopher is allowed to eat (forks semaphore released)
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
            There are three types of agents and three types of smokers in the code.
            Each agent puts two specific ingredients on the table. Each smoker is
            next in line to roll and smoke a cigarette after taking the two
            components he does not have. For example, if a smoker has paper, the
            other two components, tobacco and matches, must be on the table.

            - DEADLOCK !!
            At the beginning , This example has the possibility of deadlock.
            Imagine that the agent puts out tobacco and paper. Since the smoker with matches is waiting on tobacco,
            it might be unblocked. But the smoker with tobacco is waiting on paper,
            so it is possible (even likely) that it will also be unblocked.
            Then the first thread will block on paper and the second will block on match. Deadlock!

            Deadlock Example:
             Agent A puts “tobacco” and “paper” on the table. In this situation:
                * The smoker with “matches” (Smoker 3) wakes up because he is waiting for “tobacco” and “paper”.
                * At the same time, the smoker with “tobacco” (Smoker 2) wakes up expecting “paper”.
            However, both “tobacco” and “paper” smokers are waiting for “matches”, so both are blocked:
                * Smoker 2 waits for “match”.
                * Smoker 3 waits for “paper”.
            In this case, both smokers wait forever (deadlock occurs).

        - CODE OUTPUT !!
            Agent A put tobacco, paper
            Smoker took tobacco and paper then smoking...
            Agent A put tobacco, paper
            ...
            ...
         */

        constexpr uint8_t agentsAmount = 3;
        constexpr uint8_t smokersAmount = 3;

        std::binary_semaphore agentSem(1);
        std::binary_semaphore tobacco(0);
        std::binary_semaphore paper(0);
        std::binary_semaphore match(0);

        void execute_agent(std::binary_semaphore& _ingredients1, std::binary_semaphore& _ingredients2, std::string& agent_code, std::string& put_on_table1, std::string& put_on_table2)
        {
            while (1)
            {
                agentSem.acquire();
                std::cout << "Agent " << agent_code << " put " << put_on_table1 << ", " << put_on_table2 << std::endl;
                _ingredients1.release();
                _ingredients2.release();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        void execute_smokers(std::binary_semaphore& _ingredients1, std::binary_semaphore& _ingredients2, std::string& take_on_table1, std::string& take_on_table2)
        {
            while (1)
            {
                _ingredients1.acquire();
                _ingredients2.acquire();
                std::cout << "Smoker took " << take_on_table1 << " and " << take_on_table2 << " then smoking...\n";
                agentSem.release();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        void run()
        {
            std::string agent_code_A = "A";
            std::string agent_code_B = "B";
            std::string agent_code_C = "C";

            std::string match_staff = "match";
            std::string tobacco_staff = "tobacco";
            std::string paper_staff = "paper";

            std::array<std::thread, agentsAmount> agents = {
                    std::thread(execute_agent, std::ref(tobacco), std::ref(paper), std::ref(agent_code_A), std::ref(tobacco_staff), std::ref(paper_staff)), // Agent A
                    std::thread(execute_agent, std::ref(paper), std::ref(match), std::ref(agent_code_B), std::ref(paper_staff), std::ref(match_staff)), // Agent B
                    std::thread(execute_agent, std::ref(tobacco), std::ref(match), std::ref(agent_code_C), std::ref(tobacco_staff), std::ref(match_staff))  // Agent C
            };

            std::array<std::thread, smokersAmount> smokers = {
                    std::thread(execute_smokers, std::ref(tobacco), std::ref(paper), std::ref(tobacco_staff), std::ref(paper_staff)), // Smoker with matches
                    std::thread(execute_smokers, std::ref(paper), std::ref(match), std::ref(paper_staff), std::ref(match_staff)), // Smoker with tobacco
                    std::thread(execute_smokers, std::ref(tobacco), std::ref(match), std::ref(tobacco_staff), std::ref(match_staff))  // Smoker with paper
            };

            for (auto& agent : agents)   { if (agent.joinable())  { agent.join();  } }
            for (auto& smoker : smokers) { if (smoker.joinable()) { smoker.join(); } }
        }
    }

    namespace cigarette_smokers_parnas_solution
    {
        /*
        - Definition of the Parnas !!
        The Parnas solution is a synchronization solution for the Cigarette Smokers Problem.
        Using semaphores and a critical region, it coordinates the interactions of multiple agents
        (the material setter) and smokers (the smoker who uses the materials to smoke a cigarette).
        The logic of the solution is to synchronize between the agents who put the ingredients on the table and the smokers who pick them up and smoke.

        - The Logic of the Parnas !!
        The main goal of the Parnas solution is synchronization and deadlock prevention.
        Agents put materials on the table and smokers take these materials and smoke.
        Synchronization aims to properly coordinate an agent's placing of ingredients and a smoker's taking of those ingredients.
        It also has the goal of deadlock prevention, i.e. keeping agents and smokers working in synchronization and preventing deadlocks.

        - Steps of Parnas !!
        Agents: Each agent places a certain pair of materials (tobacco, paper, spades) on the table.
            For example, an agent can put tobacco and paper. The agent uses the agentSem semaphore to put ingredients on the table.
            This semaphore controls the process of agents putting ingredients on the table.
            Once the materials are placed on the table, the semaphores are released for the appropriate smoker to take the materials.

        Smokers: Each smoker smokes cigarettes with specific materials. The smokers wait for the respective semaphores
            to take the materials waiting on the table. The smoker acquires the two semaphores (_ingredients1 and _ingredients2)
            needed to get the required ingredients and make a cigarette. After acquiring the ingredients,
            the smoker releases the agentSem semaphore for the agents to place the next ingredient.

        Pushers: Pushers are the functions responsible for putting ingredients on the table.
            They receive semaphore signals from agents and update the materials accordingly.
            The Pusher checks what supplies are on the table and replenishes missing supplies.

        Critical Zone and Synchronization: by using mutex, multiple pushers are prevented from changing the table at the same time and
            data competitions are avoided. By using semaphores (agentSem, tobacco, paper, match), material placement and retrieval are synchronized.

        - Advantages of the Parnas Solution !!
        Deadlock Prevention: By using semaphores and mutex, it ensures that the whole system works properly and prevents deadlocks.
            This increases the reliability of the system.

        Ensuring Synchronization: It properly coordinates the interactions between agents and smokers so that all processes run in a synchronized manner.

        Simple and Effective: The solution provides a simple and effective synchronization using semaphores and critical regions.
            This both makes the code easier to understand and improves performance.

        - CODE OUTPUT !!
            Agent A put tobacco, paper
            Smoker took paper and match then made cigarette...
            Smoking...
            Agent B put paper, match
            Smoker took tobacco and paper then made cigarette...
            Smoking...
            Agent C put tobacco, matchSmoker took tobacco and match then made cigarette...
            Smoking...
            ...
            ...
         */

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
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        void execute_agent(std::binary_semaphore& _ingredients1, std::binary_semaphore& _ingredients2, std::string& agent_code, std::string& staff_on_table1, std::string& staff_on_table2)
        {
            while(true)
            {
                agentSem.acquire();
                std::cout << "Agent " << agent_code << " put " << staff_on_table1 << ", " << staff_on_table2 << std::endl;
                _ingredients1.release();
                _ingredients2.release();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        void execute_smokers(std::binary_semaphore& _mainIngredients, std::string& staff_on_table1, std::string& staff_on_table2)
        {
            while(true)
            {
                _mainIngredients.acquire();
                std::cout << "Smoker took " << staff_on_table1 << " and " << staff_on_table2 << " then made cigarette...\n";
                agentSem.release();
                std::cout << "Smoking...\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        void run()
        {
            std::string agent_code_A = "A";
            std::string agent_code_B = "B";
            std::string agent_code_C = "C";

            std::string match_staff = "match";
            std::string tobacco_staff = "tobacco";
            std::string paper_staff = "paper";

            std::array<std::thread, agentsAmount> agents = {
                    std::thread(execute_agent, std::ref(tobacco), std::ref(paper), std::ref(agent_code_A), std::ref(tobacco_staff), std::ref(paper_staff)), // Agent A
                    std::thread(execute_agent, std::ref(paper), std::ref(match), std::ref(agent_code_B), std::ref(paper_staff), std::ref(match_staff)), // Agent B
                    std::thread(execute_agent, std::ref(tobacco), std::ref(match), std::ref(agent_code_C), std::ref(tobacco_staff), std::ref(match_staff))  // Agent C
            };

            std::array<std::thread, smokersAmount> smokers = {
                    std::thread(execute_smokers, std::ref(match), std::ref(tobacco_staff), std::ref(paper_staff)), // Smoker with matches
                    std::thread(execute_smokers, std::ref(tobacco), std::ref(paper_staff), std::ref(match_staff)), // Smoker with tobacco
                    std::thread(execute_smokers, std::ref(paper), std::ref(tobacco_staff), std::ref(match_staff))  // Smoker with paper
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
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        void execute_agent(std::binary_semaphore& _ingredients1, std::binary_semaphore& _ingredients2, std::string& agent_code, std::string& staff_on_table1, std::string& staff_on_table2)
        {
            while(true)
            {
                agentSem.acquire();
                std::cout << "Agent " << agent_code << " put " << staff_on_table1 << ", " << staff_on_table2 << std::endl;
                _ingredients1.release();
                _ingredients2.release();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        void execute_smokers(std::binary_semaphore& _mainIngredients, std::string& staff_on_table1, std::string& staff_on_table2)
        {
            while(true)
            {
                _mainIngredients.acquire();
                std::cout << "Smoker took " << staff_on_table1 << " and " << staff_on_table2 << " then made cigarette...\n";
                agentSem.release();
                std::cout << "Smoking...\n";
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        void run()
        {
            std::string agent_code_A = "A";
            std::string agent_code_B = "B";
            std::string agent_code_C = "C";

            std::string match_staff = "match";
            std::string tobacco_staff = "tobacco";
            std::string paper_staff = "paper";

            std::array<std::thread, agentsAmount> agents = {
                    std::thread(execute_agent, std::ref(tobacco), std::ref(paper), std::ref(agent_code_A), std::ref(tobacco_staff), std::ref(paper_staff)), // Agent A
                    std::thread(execute_agent, std::ref(paper), std::ref(match), std::ref(agent_code_B), std::ref(paper_staff), std::ref(match_staff)), // Agent B
                    std::thread(execute_agent, std::ref(tobacco), std::ref(match), std::ref(agent_code_C), std::ref(tobacco_staff), std::ref(match_staff))  // Agent C
            };

            std::array<std::thread, smokersAmount> smokers = {
                    std::thread(execute_smokers, std::ref(match), std::ref(tobacco_staff), std::ref(paper_staff)), // Smoker with matches
                    std::thread(execute_smokers, std::ref(tobacco), std::ref(paper_staff), std::ref(match_staff)), // Smoker with tobacco
                    std::thread(execute_smokers, std::ref(paper), std::ref(tobacco_staff), std::ref(match_staff))  // Smoker with paper
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
