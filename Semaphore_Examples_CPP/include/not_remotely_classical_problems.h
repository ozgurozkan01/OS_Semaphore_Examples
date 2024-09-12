//
// Created by ozgur on 9/8/2024.
//

#ifndef SEMAPHORE_EXAMPLES_CPP_NOT_REMOTELY_CLASSICAL_PROBLEMS_H
#define SEMAPHORE_EXAMPLES_CPP_NOT_REMOTELY_CLASSICAL_PROBLEMS_H

#include <iostream>
#include <mutex>
#include <thread>
#include <array>

namespace not_remotely_classical_problems
{
    namespace sushi_bar_problem_non_solution
    {
        /*
            - PROBLEM DEFINITION !!
                Imagine a sushi bar with 5 seats. If you arrive while there is an empty seat, you can take a seat immediately.
                But if you arrive when all 5 seats are full, that means that all of them are dining together,
                and you will have to wait for the entire party to leave before you sit down

            - WHY THIS PROBLEM HAS NO SOLUTION !!
                First of all, do not think that it is a deadlock problem. Actually program runs and gives a solution,
                but this is not the result we want. The problem is, If a customer arrives while the bar is full,
                he has to give up the mutex while he waits so that other customers can leave.
                When the last customer leaves, she signals block, which wakes up at least some of the waiting customers,
                and clears must wait. But when the customers wake up, they have to get the mutex back,
                and that means they have to compete with incoming new threads. If new threads arrive and get the mutex first,
                they could take all the seats before the waiting threads. This is not just a question of injustice;
                it is possible for more than 5 threads to be in the critical section concurrently, which violates the synchronization constraints.

                In the "Little Book of Semaphore", 2 solutions were offered.

                1- one way to solve the problem is to make the departing customer, who already has the mutex, do the updating.
                2- one thread can acquire a lock and then another thread can release it.

            - CODE OUTPUT !!
                ...
                ...
                Thread with 24 id eating sushi
                Eating count : 6
                Thread with 52 id eating sushi
                Eating count : 7
                Thread with 61 id eating sushi
                Eating count : 8
                Thread with 16 id eating sushi
                Eating count : 9
                Thread with 63 id eating sushi
                Eating count : 10
                Thread with 64 id eating sushi
                Eating count : 11
                Thread with 65 id eating sushi
                Eating count : 12
                Thread with 66 id eating sushi
                Eating count : 13
                Thread with 67 id eating sushi
                Eating count : 14
                Thread with 68 id eating sushi
                Eating count : 15
                Thread with 69 id eating sushi
                Eating count : 16
                Thread with 18 id eating sushi
                Eating count : 17
                ...
                ...

               As you can see from the output, although there are 5 seats, more than 5 customers can come and eat sushi.
         */

        constexpr int seat_amount = 5;
        int eating_counter = 0; // keep track of the number of threads sitting at the bar
        int waiting_counter = 0; // keep track of the number of threads waiting
        std::mutex mutex; // mutex protects both counters
        std::counting_semaphore<5> block(0); // som incoming customers have to block on block.
        bool must_wait = false; // must wait indicates that the bar is (or has been) full

        void execute()
        {
            mutex.lock();
            if (must_wait)
            {
                waiting_counter++;
                mutex.unlock();
                block.acquire();

                mutex.lock();
                waiting_counter--;
            }

            eating_counter++;
            std::cout << "Eating count : " << eating_counter << "\n";
            must_wait = eating_counter == seat_amount;
            mutex.unlock();

            std::cout << "Thread with " << std::this_thread::get_id() << " id eating sushi\n";

            mutex.lock();
            eating_counter--;

            if (eating_counter == 0)
            {
                int signal_count = std::min(5, waiting_counter);
                // Here, after calling block.release(signal_count);, waiting threads are expected to compete to get the mutex again.
                // However, new threads can also try to get the mutex. If the new threads get the mutex before the waiting ones,
                // then more than 5 threads can enter the eating section at once.
                block.release(signal_count);
                must_wait = false;
            }
            mutex.unlock();
        }

        void run()
        {
            constexpr int customer_count = 70;
            std::array<std::thread, customer_count> customers;

            for (int i = 0; i < customer_count; ++i) { customers[i] = std::thread(execute); }
            for (int i = 0; i < customer_count; ++i) { if (customers[i].joinable()) { customers[i].join(); } }
        }
    }

    namespace sushi_bar_problem_solution_1
    {
        /*
            THAT IS THE SOLUTION 1 !!

            The only reason a waiting customer has to reacquire the mutex is to update the
            state of eating and waiting, so one way to solve the problem is to make the
            departing customer, who already has the mutex, do the updating

            - CODE OUTPUT !!
                ...
                ...
                Thread with 54 id eating sushi
                Eating count : 2
                Thread with 60 id eating sushi
                Eating count : 1
                Thread with 65 id eating sushi
                Eating count : 5
                Thread with 15 id eating sushi
                Eating count : 4
                Thread with 40 id eating sushi
                Eating count : 3
                Thread with 61 id eating sushi
                Eating count : 2
                Thread with 19 id eating sushi
                Eating count : 1
                Thread with 71 id eating sushi
                Eating count : 5
                Thread with 45 id eating sushi
                Eating count : 4
                Thread with 64 id eating sushi
                Eating count : 3
                Thread with 63 id eating sushi
                Eating count : 2
                Thread with 42 id eating sushi
                Eating count : 1
                Thread with 53 id eating sushi
                Eating count : 5
                Thread with 14 id eating sushi
                Eating count : 4
                Thread with 70 id eating sushi
                Eating count : 3
                ...
                ...
         */

        constexpr int seat_amount = 5;
        int eating_counter = 0; // keep track of the number of threads sitting at the bar
        int waiting_counter = 0; // keep track of the number of threads waiting
        std::mutex mutex; // mutex protects both counters
        std::counting_semaphore<5> block(0); // som incoming customers have to block on block.
        bool must_wait = false; // must wait indicates that the bar is (or has been) full

        void execute()
        {
            mutex.lock();

            if (must_wait)
            {
                waiting_counter++;
                mutex.unlock();
                block.acquire();
            }
            else
            {
                eating_counter++;
                must_wait = eating_counter == seat_amount;
                mutex.unlock();
            }

            std::cout << "Eating count : " << eating_counter << "\n";
            std::cout << "Thread with " << std::this_thread::get_id() << " id eating sushi\n";

            mutex.lock();
            eating_counter--;

            if (eating_counter == 0)
            {
                int signal_count = std::min(5, waiting_counter);
                waiting_counter-= signal_count;
                eating_counter += signal_count;
                must_wait = eating_counter == seat_amount;
                block.release(signal_count);
            }
            mutex.unlock();
        }

        void run()
        {
            constexpr int customer_count = 70;
            std::array<std::thread, customer_count> customers;

            for (int i = 0; i < customer_count; ++i) { customers[i] = std::thread(execute); }
            for (int i = 0; i < customer_count; ++i) { if (customers[i].joinable()) { customers[i].join(); } }
        }
    }

    namespace sushi_bar_problem_solution_2
    {
        /*
            THAT IS THE SOLUTION 2 !!

            Alternatively, solution 2 is based on the counter-intuitive idea that a lock can be transferred from one thread to another thread!
            In other words, one thread can acquire a lock and then another thread can release it.
            This is fine as long as both threads understand that the lock has been transferred.

            - CODE OUTPUT !!

         */

        constexpr int seat_amount = 5;
        int eating_counter = 0; // keep track of the number of threads sitting at the bar
        int waiting_counter = 0; // keep track of the number of threads waiting
        std::mutex mutex; // mutex protects both counters
        std::counting_semaphore<5> block(0); // som incoming customers have to block on block.
        bool must_wait = false; // must wait indicates that the bar is (or has been) full

        void execute()
        {
            mutex.lock();

            if (must_wait)
            {
                waiting_counter++;
                mutex.unlock();
                block.acquire();
                waiting_counter--;
            }

            eating_counter++;
            must_wait = eating_counter == seat_amount;
            if (waiting_counter && !must_wait)
            {
                block.release();
            }
            else
            {
                mutex.unlock();
            }

            std::cout << "Eating count : " << eating_counter << "\n";
            std::cout << "Thread with " << std::this_thread::get_id() << " id eating sushi\n";

            mutex.lock();
            eating_counter--;

            if (eating_counter == 0)
            {
                must_wait = false;
            }

            if (waiting_counter && !must_wait)
            {
                block.release();
            }
            else
            {
                mutex.unlock();
            }
        }

        void run()
        {
            constexpr int customer_count = 70;
            std::array<std::thread, customer_count> customers;

            for (int i = 0; i < customer_count; ++i) { customers[i] = std::thread(execute); }
            for (int i = 0; i < customer_count; ++i) { if (customers[i].joinable()) { customers[i].join(); } }
        }
    }

    namespace child_care_problem_non_solution
    {
        /*
            - PROBLEM DEFINITION !!
                At a child care center, state regulations require that there is always
                one adult present for every three children.

            - WHY THIS PROBLEM HAS NO SOLUTION !!
                In the book, hint says we can almost solve the problem with just one multiplex.
                multiplex counts the number of tokens available, where each token allows
                a child thread to enter. As adults enter, they signal multiplex three times; as
                they leave, they wait three times. But this is now working properly.

                Because, it causes the potential deadlock. It's not always the case. Imagine that there are three children
                and two adults in the child care center. The value of multiplex is 3, so either
                adult should be able to leave. But if both adults start to leave at the same time,
                they might divide the available tokens between them, and both block.
         */

        constexpr int child_count_per_adult = 3;
        constexpr int adult_count = 2;
        constexpr int child_count_in_center = 3;


        std::counting_semaphore<child_count_in_center> children_per_adult_sem(0); // multiplex

        void execute_adult()
        {
            children_per_adult_sem.release(child_count_per_adult);
            // Critical Section
            children_per_adult_sem.acquire();
            children_per_adult_sem.acquire();
            children_per_adult_sem.acquire();
        }

        void run()
        {
            std::array<std::thread, adult_count> adults;

            for (int i = 0; i < adult_count; ++i) { adults[i] = std::thread(execute_adult); }
            for (int i = 0; i < adult_count; ++i) { if (adults[i].joinable()) { adults[i].join(); } }
        }
    }

    namespace room_party_problem
    {
        /*
            - CONSTRAINTS ABOUT PROBLEM !!
                1. Any number of students can be in a room at the same time.
                2. The Dean of Students can only enter a room if there are no students in
                the room (to conduct a search) or if there are more than 50 students in
                the room (to break up the party).
                3. While the Dean of Students is in the room, no additional students may
                enter, but students may leave.
                4. The Dean of Students may not leave the room until all students have left.
                5. There is only one Dean of Students, so you do not have to enforce exclusion
                among multiple deans.
         */

        enum DeanState
        {
            not_here,
            waiting,
            in_the_room
        };

        constexpr int min_students_dean_enter_room = 50;
        int student_counter = 0; // counts the number students in the room
        DeanState dean_state = not_here; // dean is the state of the Dean
        std::mutex mutex; // mutex protects students and dean, so this is yet another example of a scoreboard.
        std::mutex turn; //  turnstile that keeps students from entering while the Dean is in the room
        std::counting_semaphore<1> clear(0); // Ensures Dean leaves only after all students have left
        std::counting_semaphore<1> lie_in(0); // Rendezvous between student and the Dean (not explicitly used here)

        void execute_dean()
        {
            while (1)
            {
                /*
                     When the Dean arrives, there are three cases: if there are students in the
                     room, but not 50 or more, the Dean has to wait. If there are 50 or more, the
                     Dean breaks up the party and waits for the students to leave. If there are no
                     students, the Dean searches and leaves.

                     In the first two cases, the Dean has to wait for a rendezvous with a student,
                     so he has to give up mutex to avoid a deadlock. When the Dean wakes up, he
                     has to modify the scoreboard, so he needs to get the mutex back.
                 */

                mutex.lock();

                if (student_counter > 0 && student_counter < min_students_dean_enter_room)
                {
                    dean_state = waiting;
                    std::cout << "Dean is waiting in the party...\n";
                    mutex.unlock();
                    lie_in.acquire();
                }

                // Students must be 0 or >= 50
                if (student_counter >= min_students_dean_enter_room)
                {
                    dean_state = in_the_room;
                    // break_up()
                    std::cout << "Dean broke up the party and waiting for students to leave...\n";
                    turn.lock();
                    mutex.unlock();
                    clear.acquire();
                    turn.unlock();
                }
                else // Student count = 0
                {
                    // search()
                    std::cout << "Dean is searching the room...\n";
                    std::cout << "Dean got out the room...\n";
                    exit(-1);
                }

                dean_state = not_here;
                mutex.unlock();
            }
        }

        void execute_student(const int& student_code)
        {
            while (1)
            {
                /*
                    There are three cases where a student might have to signal the Dean. If
                    the Dean is waiting, then the 50th student in or the last one out has to signal
                    lieIn. If the Dean is in the room (waiting for all the students to leave), the last
                    student out signals clear. In all three cases, it is understood that the mutex
                    passes from the student to the Dean.

                    One part of this solution that may not be obvious is how we know at Dean’s code
                    that students must be 0 or not less than 50. The key is to realize that there are
                    only two ways to get to this point: either the first conditional was false,
                    which means that students is either 0 or not less than 50; or the Dean was waiting on
                    lieIn when a student signaled, which means, again, that students is either 0 or not less than 50
                 */

                mutex.lock();

                if (dean_state == in_the_room)
                {
                    mutex.unlock();
                    turn.lock();
                    turn.unlock();
                    mutex.lock();
                }

                student_counter++;

                if (student_counter == min_students_dean_enter_room && dean_state == waiting)
                {
                    lie_in.release(); // Signal the Dean that student count reached to 50
                }
                else
                {
                    mutex.unlock();
                }

                std::cout << "Student has " << student_code << " number is partying...\n";

                mutex.lock();

                student_counter--;

                if (student_counter == 0 && dean_state == waiting)
                {
                    lie_in.release(); // Signal the Dean that all students have left
                }
                else if (student_counter == 0 && dean_state == in_the_room)
                {
                    clear.release(); // Signal the Dean that all students have left the room
                }
                else
                {
                    mutex.unlock();
                }
            }
        }

        void run()
        {
            constexpr int student_count = 250;
            std::array<std::thread, student_count> students;

            for (int i = 0; i < student_count; ++i) { students[i] = std::thread(execute_student, i); }

            std::thread dean(execute_dean);

            for (int i = 0; i < student_count; ++i) { if (students[i].joinable()) { students[i].join(); } }

            if (dean.joinable()) { dean.join(); }
        }
    }

    namespace senate_bus_problem_solution1
    {
        /*
            - PROBLEM DEFINITION !!
                Riders come to a bus stop and wait for a bus. When the bus arrives, all the waiting
                riders invoke boardBus, but anyone who arrives while the bus is boarding has
                to wait for the next bus. The capacity of the bus is 50 people; if there are more
                than 50 people waiting, some will have to wait for the next bus.
                When all the waiting riders have boarded, the bus can invoke depart. If the
                bus arrives when there are no riders, it should depart immediately

            - PASS THE BATON PATTERN
                It is a synchronization model used in concurrent programming and multithreading environments.
                This model is used to delegate a resource or a critical region between threads.
                The goal is to enable one thread to efficiently delegate a resource or lock to another thread.

            - PATTERN PURPOSE
            The purpose of this pattern is to minimize the use of synchronization tools such as locks (mutex or semaphore)
            by delegating control or resources between threads. This avoids unnecessary holding of locks and reduces waiting times.
         */

        constexpr int max_rider_count_per_bus = 50;
        constexpr int total_bus_count = 3;
        constexpr int total_rider_count = 50;

        int rider_count = 0;
        std::mutex mutex;
        std::counting_semaphore<max_rider_count_per_bus> multiplex(max_rider_count_per_bus);
        std::counting_semaphore<total_bus_count> bus(0);
        std::counting_semaphore<total_rider_count> all_aboard(0);

        void execute_bus()
        {
            /*
                 When the bus arrives, it gets mutex, which prevents late arrivals from entering the boarding area.
                 If there are no riders, it departs immediately. Otherwise, it signals bus and waits for the riders to board
             */
            while (true)
            {
                mutex.lock();

                if (rider_count > 0)
                {
                    bus.release();
                    all_aboard.acquire();
                }
                mutex.unlock();

                std::cout << "Bus is departing...\n";
            }
        }

        void execute_rider()
        {
            /*
                The multiplex controls the number of riders in the waiting area, although
                strictly speaking, a rider doesn’t enter the waiting area until she increments
                riders.

                Riders wait on bus until the bus arrives. When a rider wakes up, it is
                understood that she has the mutex. After boarding, each rider decrements
                riders. If there are more riders waiting, the boarding rider signals bus and
                pass the mutex to the next rider. The last rider signals allAboard and passes
                the mutex back to the bus. Finally, the bus releases the mutex and departs.
             */
            while (true)
            {
                multiplex.acquire();
                mutex.lock();
                rider_count++;
                mutex.unlock();
                bus.acquire();
                multiplex.release();

                std::cout << "Boarding the bus...\n";

                rider_count--;

                if (rider_count == 0)
                {
                    all_aboard.release();
                }
                else
                {
                    bus.release();
                }
            }
        }

        void run()
        {
            std::thread rider_threads[total_rider_count];
            std::thread bus_threads[total_bus_count];

            for (int i = 0; i < total_bus_count; ++i) { bus_threads[i] = std::thread(execute_bus); }
            for (int i = 0; i < total_rider_count; ++i) { rider_threads[i] = std::thread(execute_rider); }

            for (auto& t : bus_threads) { if (t.joinable()) { t.join(); } }
            for (auto& t : rider_threads) { if (t.joinable()) { t.join(); } }
        }
    }

    namespace senate_bus_problem_solution2
    {
        /*
            - PROBLEM DEFINITION !!
                Riders come to a bus stop and wait for a bus. When the bus arrives, all the waiting
                riders invoke boardBus, but anyone who arrives while the bus is boarding has
                to wait for the next bus. The capacity of the bus is 50 people; if there are more
                than 50 people waiting, some will have to wait for the next bus.
                When all the waiting riders have boarded, the bus can invoke depart. If the
                bus arrives when there are no riders, it should depart immediately

            - I'll DO IT FOR YOU PATTERN !!
                It is a pattern often used in parallel programming or synchronization scenarios,
                where a component that performs an operation must also take over the work that needs to be done on behalf of another component.

            - PATTERN PURPOSE !!
                The main purpose of this pattern is that a thread that completes a certain part of a job will take over the rest of the
                work that other threads need to do. This pattern can speed up the job completion process and avoid unnecessary synchronization waits.

            - CHALLENGE !!
                if riders arrive while the bus is boarding, they might be annoyed if you make them wait for the next one.
                Can you find a solution that allows late arrivals to board without violating the other constraints?
         */

        constexpr int max_rider_count_per_bus = 50;
        constexpr int total_bus_count = 3;
        constexpr int total_rider_count = 50;

        int waiting = 0; // waiting is the number of riders in the boarding area which is protected by mutex
        std::mutex mutex; // protects the waiting value
        std::counting_semaphore<total_bus_count> bus(0); // signals when the bus has arrived
        std::counting_semaphore<max_rider_count_per_bus> boarded(0); // signals that a rider has boarded.

        void execute_bus()
        {
            /*
                The bus gets the mutex and holds it throughout the boarding process. The
                loop signals each rider in turn and waits for her to board. By controlling the
                number of signals, the bus prevents more than 50 riders from boarding.
                When all the riders have boarded, the bus updates waiting, which is an
                example of the “I’ll do it for you” pattern
             */
            while (true)
            {
                mutex.lock();

                int n = std::min(waiting, max_rider_count_per_bus);

                for (int i = 0; i < n; ++i)
                {
                    bus.release();
                    boarded.acquire();
                }

                waiting = std::max(waiting - max_rider_count_per_bus, 0);
                mutex.unlock();

                // depart()
                std::cout << "depart" << std::endl;
            }
        }

        void execute_rider()
        {
            while (true)
            {
                mutex.lock();
                waiting++;
                mutex.unlock();

                bus.acquire();
                // board()
                std::cout << "board" << std::endl;
                boarded.release();
            }
        }

        void run()
        {
            std::thread rider_threads[total_rider_count];
            std::thread bus_threads[total_bus_count];

            for (int i = 0; i < total_bus_count; ++i) { bus_threads[i] = std::thread(execute_bus); }
            for (int i = 0; i < total_rider_count; ++i) { rider_threads[i] = std::thread(execute_rider); }

            for (auto& t : bus_threads) { if (t.joinable()) { t.join(); } }
            for (auto& t : rider_threads) { if (t.joinable()) { t.join(); } }
        }
    }

    namespace senate_bus_problem_challenge_solution
    {
        // NOT IMPLEMENTED YET
    }

    namespace faneuil_hall_problem
    {
        /*
            - PROBLEM DEFINITION !!
                “There are three kinds of threads: immigrants, spectators, and a one judge.
                Immigrants must wait in line, check in, and then sit down. At some point, the
                judge enters the building. When the judge is in the building, no one may enter,
                and the immigrants may not leave. Spectators may leave. Once all immigrants
                check in, the judge can confirm the naturalization. After the confirmation, the
                immigrants pick up their certificates of U.S. Citizenship. The judge leaves at
                some point after the confirmation. Spectators may now enter as before. After
                immigrants get their certificates, they may leave.”

            - CONSTRAINTS !!
                • Immigrants must invoke enter, checkIn, sitDown, swear,
                    getCertificate and leave.
                • The judge invokes enter, confirm and leave.
                • Spectators invoke enter, spectate and leave.
                • While the judge is in the building, no one may enter and immigrants may
                    not leave.
                • The judge can not confirm until all immigrants who have invoked enter
                    have also invoked checkIn.
                • Immigrants can not getCertificate until the judge has executed
                    confirm

            - NOTE !!
                In this solution it is possible for immigrants to get stuck, after they
                get their certificate, by another judge coming to swear in the next batch of
                immigrants. If that happens, they might have to wait through another swearing
                in-ceremony.
         */

        constexpr int immigrant_count = 5;
        constexpr int spectator_count = 3;

        std::mutex no_judge; // noJudge acts as a turnstile for incoming immigrants and spectators;
        int entered_immigrant; // which counts the number of immigrants in the room
        int checked_immigrant; // counts the number of immigrants who have checked in
        std::mutex mutex; // protects checked value control
        std::counting_semaphore<immigrant_count> confirmed(0); //  signals that the judge has executed confirm.
        std::binary_semaphore all_signed_in(0);
        bool is_judge = false;


        void execute_immigrant()
        {
            /*
                Immigrants pass through a turnstile when they enter; while the judge is in
                the room, the turnstile is locked.
                After entering, immigrants have to get mutex to check in and update
                checked. If there is a judge waiting, the last immigrant to check in signals
                allSignedIn and passes the mutex to the judge
             */

            no_judge.lock();
            std::cout << "Immigrant entered the room...\n"; // enter()
            entered_immigrant++;
            no_judge.unlock();

            mutex.lock();
            std::cout << "Immigrant checked in...\n"; // check();
            checked_immigrant++;

            if (is_judge == 1 && entered_immigrant == checked_immigrant)
            {
                all_signed_in.release();
            }
            else
            {
                mutex.unlock();
            }

            std::cout << "Immigrant had a seat...\n"; // sit_down()
            confirmed.acquire();
            std::cout << "Immigrant sweared...\n"; // swear()
            std::cout << "Immigrant got certificate...\n"; // get_certificate()

            no_judge.lock();
            std::cout << "Immigrant is leaving...\n"; // leave()
            no_judge.unlock();

        }
        void execute_judge()
        {
            /*
                The judge holds noJudge to bar immigrants and spectators from entering,
                and mutex so he can access entered and checked.

                If the judge arrives at an instant when everyone who has entered has also
                checked in, she can proceed immediately. Otherwise, she has to give up the
                mutex and wait. When the last immigrant checks in and signals allSignedIn,
                it is understood that the judge will get the mutex back.

                After invoking confirm, the judge signals confirmed once for every immigrant who has checked in,
                and then resets the counters (an example of “I’ll do it for you”).
                Then the judge leaves and releases mutex and noJudge.

                After the judge signals confirmed, immigrants invoke swear and
                getCertificate concurrently, and then wait for the noJudge turnstile to open
                before leaving.
             */

            no_judge.lock();
            mutex.lock();

            std::cout << "Judge entered the room...\n"; // enter()
            is_judge = true;

            if (entered_immigrant > checked_immigrant)
            {
                mutex.unlock();
                all_signed_in.acquire();
            }
            std::cout << "Judge confirmed all checked...\n"; // confirm()

            confirmed.release(checked_immigrant);
            entered_immigrant = 0;
            checked_immigrant = 0;

            std::cout << "Judge is leaving the room...\n" ;// leave()
            is_judge = false;
            mutex.unlock();
            no_judge.unlock();
        }
        void execute_spectator()
        {
            no_judge.lock();
            std::cout << "Spectator entered the room...\n"; // enter()
            no_judge.unlock();

            std::cout << "Spectator is spectating...\n" ;// spectate()
            std::cout << "Spectator is leaving...\n" ;// leave()
        }
        void run()
        {
            std::vector<std::thread> threads;

            for (int i = 0; i < immigrant_count; ++i) { threads.emplace_back(execute_immigrant); }
            threads.emplace_back(execute_judge);
            for (int i = 0; i < spectator_count; ++i) { threads.emplace_back(execute_spectator); }

            for (auto& t : threads) { if (t.joinable()) { t.join(); } }
        }
    }

    namespace faneuil_hall_problem_puzzle_solution
    {

    }

    namespace extended_faneuil_hall_problem
    {
        constexpr int immigrant_count = 5;
        constexpr int spectator_count = 3;

        std::mutex no_judge; // noJudge acts as a turnstile for incoming immigrants and spectators;
        int entered_immigrant; // which counts the number of immigrants in the room
        int checked_immigrant; // counts the number of immigrants who have checked in
        std::mutex mutex; // protects checked value control
        std::counting_semaphore<immigrant_count> confirmed(0); //  signals that the judge has executed confirm.
        std::binary_semaphore all_signed_in(0);
        std::binary_semaphore all_gone(0);
        std::binary_semaphore exit(0);
        bool is_judge = false;


        void execute_immigrant()
        {
            /*
                When the judge is ready to leave, she can’t release noJudge, because that would allow more immigrants,
                and possibly another judge, to enter. Instead, she signals exit, which allows one immigrant to leave, and passes mutex.

                The immigrant that gets the signal decrements checked and then passes the baton to the next immigrant.
                The last immigrant to leave signals allGone and passes the mutex back to the judge.
                This pass-back is not strictly necessary, but it has the nice feature that the judge releases both mutex
                and noJudge to end the phase cleanly
             */
            no_judge.lock();
            std::cout << "Immigrant entered the room...\n"; // enter()
            entered_immigrant++;
            no_judge.unlock();

            mutex.lock();
            std::cout << "Immigrant checked in...\n"; // check();
            checked_immigrant++;

            if (is_judge == 1 && entered_immigrant == checked_immigrant)
            {
                all_signed_in.release();
            }
            else
            {
                mutex.unlock();
            }

            std::cout << "Immigrant had a seat...\n"; // sit_down()
            confirmed.acquire();
            std::cout << "Immigrant sweared...\n"; // swear()
            std::cout << "Immigrant got certificate...\n"; // get_certificate()

            exit.acquire();
            std::cout << "Immigrant is leaving the room...\n" ;// leave()
            checked_immigrant--;

            if (checked_immigrant == 0)
            {
                all_gone.release();
            }
            else
            {
                exit.release();
            }
        }
        void execute_judge()
        {
            no_judge.lock();
            mutex.lock();

            std::cout << "Judge entered the room...\n"; // enter()
            is_judge = true;

            if (entered_immigrant > checked_immigrant)
            {
                mutex.unlock();
                all_signed_in.acquire();
            }
            std::cout << "Judge confirmed all checked...\n"; // confirm()

            confirmed.release(checked_immigrant);
            entered_immigrant = 0;

            std::cout << "Judge is leaving the room...\n" ;// leave()
            is_judge = false;

            exit.release();
            all_gone.acquire();
            mutex.unlock();
            no_judge.unlock();
        }
        void execute_spectator()
        {
            no_judge.lock();
            std::cout << "Spectator entered the room...\n"; // enter()
            no_judge.unlock();

            std::cout << "Spectator is spectating...\n" ;// spectate()
            std::cout << "Spectator is leaving...\n" ;// leave()
        }
        void run()
        {
            std::vector<std::thread> threads;

            for (int i = 0; i < immigrant_count; ++i) { threads.emplace_back(execute_immigrant); }
            threads.emplace_back(execute_judge);
            for (int i = 0; i < spectator_count; ++i) { threads.emplace_back(execute_spectator); }

            for (auto& t : threads) { if (t.joinable()) { t.join(); } }
        }
    }

    namespace dining_hall_problem
    {
        /*
            - PROBLEM DEFINITION !!
                Students in the dining hall invoke dine and then leave. After invoking dine
                and before invoking leave a student is considered “ready to leave”.
                The synchronization constraint that applies to students is that, in order to
                maintain the illusion of social suave, a student may never sit at a table alone. A
                student is considered to be sitting alone if everyone else who has invoked dine
                invokes leave before she has finished dine

                If you analyze the constraints, you will realize that there is only one situation
                where a student has to wait, if there is one student eating and one student who
                wants to leave. But there are two ways to get out of this situation: another
                student might arrive to eat, or the dining student might finish.
                In either case, the student who signals the waiting student updates the
                counters, so the waiting student doesn’t have to get the mutex back. This
                is another example of the the “I’ll do it for you” pattern.

                When is student is checking in, if she sees one student eating and one waiting
                to leave, she lets the waiter off the hook and decrements readyToLeave for him.

                After dining, the student checks three cases:
                • If there is only one student left eating, the departing student has to give
                    up the mutex and wait.
                • If the departing student finds that someone is waiting for her, she signals
                    him and updates the counter for both of them.
                • Otherwise, she just decrements readyToLeave and leaves.
         */

        constexpr int student_count = 7;

        int eating_counter = 0;
        int ready_to_leave_counter = 0;
        std::mutex mutex;
        std::binary_semaphore ok_to_leave(0);

        void execute(const int& student_code)
        {
            std::cout << student_code << ". student is getting food...\n";

            mutex.lock();
            eating_counter++;

            if (eating_counter == 2 && ready_to_leave_counter == 1)
            {
                ok_to_leave.release();
                ready_to_leave_counter--;
            }

            mutex.unlock();

            std::cout << student_code << ". student is dining...\n";

            mutex.lock();
            eating_counter--;
            ready_to_leave_counter++;

            if (eating_counter == 1 && ready_to_leave_counter == 1)
            {
                mutex.unlock();
                ok_to_leave.acquire();
            }
            else if (eating_counter == 0 && ready_to_leave_counter == 2)
            {
                ok_to_leave.release();
                ready_to_leave_counter -= 2;
                mutex.unlock();
            }
            else
            {
                ready_to_leave_counter--;
                mutex.unlock();
            }

            std::cout << student_code << ". student is leaving...\n";
        }
        void run()
        {
            std::array<std::thread, student_count> students;

            for (int i = 0; i < student_count; ++i) { students[i] = std::thread(execute, i); }
            for (int i = 0; i < student_count; ++i) { if (students[i].joinable()) { students[i].join(); } }
        }
    }

    namespace extended_dining_hall_problem
    {
        /*
            - PROBLEM DEFINITION !!
                The Dining Hall problem gets a little more challenging if we add another step.
                As students come to lunch they invoke getFood, dine and then leave. After
                invoking getFood and before invoking dine, a student is considered “ready to
                eat”. Similarly, after invoking dine a student is considered “ready to leave”.
                The same synchronization constraint applies: a student may never sit at a
                table alone. A student is considered to be sitting alone if either
                • She invokes dine while there is no one else at the table and no one ready
                to eat, or
                • everyone else who has invoked dine invokes leave before she has finished
                dine

                Again, if we analyze the constraints, we realize that there is only one situation
                where a student who is ready to eat has to wait, if there is no one eating and
                no one else ready to eat. And the only way out is if someone else arrives who is
                ready to eat.

            - PAY ATTENTION !!
                As in the previous solution, It is used the “I’ll do it for you” pattern so that a
                waiting student doesn’t have to get the mutex back.

                The primary difference between this solution and the previous one is that
                the first student who arrives at an empty table has to wait, and the second
                student allows both students to proceed. It either case, we don’t have to check
                for students waiting to leave, since no one can leave an empty table!
         */
    }
}

#endif //SEMAPHORE_EXAMPLES_CPP_NOT_REMOTELY_CLASSICAL_PROBLEMS_H
