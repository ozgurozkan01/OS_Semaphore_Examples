//
// Created by ozgur on 9/7/2024.
//

#ifndef SEMAPHORE_EXAMPLES_CPP_NOT_SO_CLASSICAL_PROBLEMS_H
#define SEMAPHORE_EXAMPLES_CPP_NOT_SO_CLASSICAL_PROBLEMS_H

#include <iostream>
#include <mutex>
#include <thread>
#include <array>
#include <list>
#include "single_linked_list.h"

namespace not_so_classical_problems
{
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
        int counter;
        std::mutex mutex;
    };

    namespace  search_insert_delete_problem
    {
        // To show properly how it is running, added single linked list to the problem.

        /*
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
                24 is inserted to the list...
                41 could not find in the list by searching...
                6 could not find in the list for deleting...
                34 could not find in the list by searching...
                32 is inserted to the list...
                1 could not find in the list for deleting...
                43 could not find in the list by searching...
                26 is inserted to the list...
                12 could not find in the list for deleting...
                16 could not find in the list by searching...
                10 could not find in the list for deleting...
                24 is inserted to the list...
         */

        // GLOBAL VARIABLES
        std::mutex insert_mutex;
        std::mutex no_searcher;
        std::mutex no_inserter;
        Lightswitch search_switch;
        Lightswitch insert_switch;
        SinglyLinkedList test_list;

        void execute_searcher()
        {
            while (true)
            {
                search_switch.lock(no_searcher);
                // CRITICAL SECTION
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, 50);

                int searching_value = dis(gen);
                bool is_found = test_list.search(searching_value);

                if (is_found) { std::cout << searching_value << " exists in the list...\n"; }
                else { std::cout << searching_value << " could not find in the list by searching...\n"; }

                search_switch.unlock(no_searcher);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }
        void execute_inserter()
        {
            while (true)
            {
                insert_switch.lock(no_inserter);
                insert_mutex.lock();
                // CRITICAL SECTION
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, 50);

                int appending_value = dis(gen);
                test_list.append(appending_value);

                std::cout << appending_value << " is inserted to the list...\n";

                insert_mutex.unlock();
                insert_switch.unlock(no_inserter);
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }
        void execute_deleter()
        {
            while (true)
            {
                no_searcher.lock();
                no_inserter.lock();
                // CRITICAL SECTION
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, 50);

                int deleting_value = dis(gen);
                bool is_deleted = test_list.deleteValue(deleting_value);

                if (is_deleted) { std::cout << deleting_value << " is deleted from the list...\n"; }
                else { std::cout << deleting_value << " could not find in the list for deleting...\n"; }

                no_inserter.unlock();
                no_searcher.unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            }
        }

        void run()
        {
            constexpr int thread_amount = 3;
            std::array<std::thread, thread_amount> threads {
                    std::thread(execute_searcher),
                    std::thread(execute_inserter),
                    std::thread(execute_deleter)
            };

            for(auto& thread : threads) { if (thread.joinable()) { thread.join(); } }
        }
    }

    namespace  unisex_bathroom_problem
    {
        /*
            - PROBLEM DEFINITION !!
            For the comfort of employees in a company, it has been decided to place a unisex bathroom in a strategically convenient location.
            However, there are some constraints and conditions for the use of this bathroom.

            - CONSTRAINTS !!
                • There cannot be men and women in the bathroom at the same time.
                • There should never be more than three employees squandering company
                time in the bathroom.

            - ATTENTION !!
            At this problem, there is a starvation problem. So how ? If there are women in the bathroom - does not matter it is full or not-
            and also if there are men waiting for to get in in front of the door, men never might get in the bathroom. Because men and women
            cannot be in bathroom at the same time, when women are in the bathroom, if a queue of women forms at the door, men will never
            be able to enter the bathroom if there is at least one woman in the bathroom, and women waiting in the queue at the door will be able to enter in turn.

            - SOLUTION !!
            Using turnstile to prevent starvation.

            - CODE OUTPUT !!
                A male has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom

                As you can see in the output, women can very often enter the bathroom and prevent men from entering.
         */

        std::mutex empty; // empty is 1 (unlock) if the room is empty and 0 (lock) otherwise.
        Lightswitch maleSwitch;
        Lightswitch femaleSwitch;
        // ATTENTION : men and women cannot get in at the same time
        std::counting_semaphore<3> maleMultiplex(3); // 3 men can get in at the same time
        std::counting_semaphore<3> femaleMultiplex(3); // 3 women can get in at the same time

        void execute_male()
        {
            while (true)
            {
                maleSwitch.lock(empty);
                maleMultiplex.acquire();
                // bathroom code here
                std::cout << "A male has entered to the bathroom\n";
                maleMultiplex.release();
                maleSwitch.unlock(empty);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }

        void execute_female()
        {
            while (true)
            {
                femaleSwitch.lock(empty);
                femaleMultiplex.acquire();
                // bathroom code here
                std::cout << "A female has entered to the bathroom\n";
                femaleMultiplex.release();
                femaleSwitch.unlock(empty);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }

        void run()
        {
            constexpr int female_number = 5;
            constexpr int male_number = 5;
            std::array<std::thread, female_number>  females;
            std::array<std::thread, male_number>    males;

            for (int i = 0; i < male_number; ++i) { males[i] = std::thread(execute_male); }
            for (int i = 0; i < female_number; ++i) { females[i] = std::thread(execute_female); }

            for (auto& male : males) { if (male.joinable()) { male.join(); }}
            for (auto& female : females) { if (female.joinable()) { female.join(); }}
        }
    }

    namespace no_starve_unisex_bathroom_problem
    {
        /*
                - WITH TURNSTILE vs WITHOUT TURNSTILE !!
                When a turnstile is used, men and women are forced to enter the bathroom in turn.
                The first gender to arrive first gets a turn to enter the bathroom and
                the other gender is blocked from entering the bathroom until they leave.

                When the turnstile is not used, men and women may try to enter the bathroom
                at the same time using their own semaphores, which can sometimes lead to race conditions.

                - CODE OUTPUT !!
                A male has entered to the bathroom
                A male has entered to the bathroom
                A male has entered to the bathroom
                A female has entered to the bathroom
                A male has entered to the bathroom
                A female has entered to the bathroom
                A female has entered to the bathroom
                ...
                ...
         */

        std::mutex empty; // empty is 1 (unlock) if the room is empty and 0 (lock) otherwise.
        Lightswitch maleSwitch;
        Lightswitch femaleSwitch;
        // ATTENTION : men and women cannot get in at the same time
        std::counting_semaphore<3> maleMultiplex(3); // 3 men can get in at the same time
        std::counting_semaphore<3> femaleMultiplex(3); // 3 women can get in at the same time
        std::mutex turnstile;

        void execute_male()
        {
            while (true)
            {
                turnstile.lock();
                maleSwitch.lock(empty);
                turnstile.unlock();

                maleMultiplex.acquire();
                // bathroom code here
                std::cout << "A male has entered to the bathroom\n";
                maleMultiplex.release();
                maleSwitch.unlock(empty);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }

        void execute_female()
        {
            while (true)
            {
                turnstile.lock();
                femaleSwitch.lock(empty);
                turnstile.unlock();

                femaleMultiplex.acquire();
                // bathroom code here
                std::cout << "A female has entered to the bathroom\n";
                femaleMultiplex.release();
                femaleSwitch.unlock(empty);
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        }

        void run()
        {
            constexpr int female_number = 5;
            constexpr int male_number = 5;
            std::array<std::thread, female_number>  females;
            std::array<std::thread, male_number>    males;

            for (int i = 0; i < male_number; ++i) { males[i] = std::thread(execute_male); }
            for (int i = 0; i < female_number; ++i) { females[i] = std::thread(execute_female); }

            for (auto& male : males) { if (male.joinable()) { male.join(); }}
            for (auto& female : females) { if (female.joinable()) { female.join(); }}
        }
    }

    namespace modus_hall_problem
    {
        /*
            - PROBLEM DEFINITION !!
                This problem deals with the management of the right of way between two different groups on a passageway.
                The passageway is too narrow to allow two people to pass side by side, so when two people meet, only one of them can pass.

            - SCENARIO !!
                * Groups: There are two groups: Group A (the “heathens” from Modus Hall) and Group B (the “prudes” from West Hall).
                * Passageway: A narrow passageway is where the two groups meet. The passageway allows only one person to pass.
                * Critical Area: The passageway must be controlled between both groups based on majority rule.

            - CONDITIONS !!
            As each student checks in, he has to consider the following cases:
                • If the field is empty, the student lays claim for the heathens.
                • If the heathens currently in charge, but the new arrival has tipped the
                    balance, he locks the prude turnstile and the system switches to transition
                    mode.
                • If the prudes in charge, but the new arrival doesn’t tip the balance, he
                    joins the queue.
                • If the system is transitioning to heathen control, the new arrival joins the
                    queue.
                • Otherwise we conclude that either the heathens are in charge, or the system is transitioning to prude control. In either case, this thread can
                    proceed.

            Similarly, as each student checks out, she has to consider several cases.

                • If she is the last heathen to check out, she has to consider the following:
                    – If the system is in transition, that means that the prude turnstile is
                        locked, so she has to open it.
                    – If there are prudes waiting, she signals them and updates status so
                        the prudes are in charge. If not, the new status is ’neutral’.
                • If she is not the last heathen to check out, she still has to check the
                    possibility that her departure will tip the balance. In that case, she closes
                    the heathen turnstile and starts the transition.

            - ATTENTION !!
                One potential difficulty of this solution is that any number of threads could
                be interrupted at "Risk Line" , where they would have passed the turnstile but not yet
                checked in. Until they check in, they are not counted, so the balance of power
                may not reflect the number of threads that have passed the turnstile. Also,
                a transition ends when all the threads that have checked in have also checked
                out. At that point, there may be threads (of both types) that have passed the
                turnstile.

                These behaviors may affect efficiency—this solution does not guarantee maximum concurrency
                but they don’t affect correctness, if you accept that “majority rule” only applies to threads that have registered to vote.


            - CODE OUTPUT (In my IDE code runs without stucking on DEBUG mode)
                Heathen is crossing the field.
                Heathen is crossing the field.
                Heathen is crossing the field.
                Prude is crossing the field.
                Prude is crossing the field.
                Prude is crossing the field.
                Prude is crossing the field.
                Heathen is crossing the field.
                Heathen is crossing the field.
                Prude is crossing the field.
                --------- * -------- * --------- *

                Normal Running
                Heathen is crossing the field.
                Heathen is crossing the field.
                Prude is crossing the field.
                Prude is crossing the field.
                Prude is crossing the field.
                Prude is crossing the field.
                Prude is crossing the field.
                (STUCKING)
        */

        enum Status
        {
            neutral,
            heathens_rule,
            prudes_rule,
            transition_to_heathens,
            transition_to_prudes
        };

        int heathen_counter = 0;
        int prude_counter = 0;
        Status field_statu = neutral;
        std::mutex mutex;
        std::mutex heathen_turn; // control access to the field so that we can bar one side or the other during a transition.
        std::mutex prude_turn; // control access to the field so that we can bar one side or the other during a transition.
        std::counting_semaphore<1> heathen_queue(0); // where threads wait after checking in and before taking the field.
        std::counting_semaphore<1> prude_queue(0); // where threads wait after checking in and before taking the field.

        void execute_heathen()
        {
            heathen_turn.lock();
            heathen_turn.unlock();
            // Risk Line
            mutex.lock();
            heathen_counter++;

            switch (field_statu)
            {
                case neutral:
                    field_statu = heathens_rule;
                    mutex.unlock();
                    break;
                case prudes_rule:
                    if (heathen_counter > prude_counter)
                    {
                        field_statu = transition_to_heathens;
                        prude_turn.lock();
                    }
                    mutex.unlock();
                    heathen_queue.acquire();
                    break;
                case transition_to_heathens:
                    mutex.unlock();
                    heathen_queue.acquire();
                    break;
                default:
                    mutex.unlock();
            }

            std::cout << "Heathen is crossing the field.\n";

            mutex.lock();
            heathen_counter--;

            if (heathen_counter == 0)
            {
                if (field_statu == transition_to_prudes)
                {
                    prude_turn.unlock();
                }

                if (prude_counter > 0)
                {
                    prude_queue.release(prude_counter);
                    field_statu = prudes_rule;
                }
                else
                {
                    field_statu = neutral;
                }
            }

            if (field_statu == heathens_rule && prude_counter > heathen_counter)
            {
                field_statu = transition_to_prudes;
                heathen_turn.lock();
            }

            mutex.unlock();
        }

        void execute_prude()
        {
            prude_turn.lock();
            prude_turn.unlock();
            // Risk Line
            mutex.lock();
            prude_counter++;

            switch (field_statu)
            {
                case neutral:
                    field_statu = prudes_rule;
                    mutex.unlock();
                    break;
                case heathens_rule:
                    if (prude_counter > heathen_counter)
                    {
                        field_statu = transition_to_prudes;
                        heathen_turn.lock();
                    }
                    mutex.unlock();
                    prude_queue.acquire();
                    break;
                case transition_to_prudes:
                    mutex.unlock();
                    prude_queue.acquire();
                    break;
                default:
                    mutex.unlock();
            }

            std::cout << "Prude is crossing the field.\n";

            mutex.lock();
            prude_counter--;

            if (prude_counter == 0)
            {
                if (field_statu == transition_to_heathens)
                {
                    heathen_turn.unlock();
                }

                if (heathen_counter > 0)
                {
                    heathen_queue.release(heathen_counter);
                    field_statu = heathens_rule;
                }
                else
                {
                    field_statu = neutral;
                }
            }

            if (field_statu == prudes_rule && heathen_counter > prude_counter)
            {
                field_statu = transition_to_heathens;
                prude_turn.lock();
            }

            mutex.unlock();
        }

        void run()
        {
            std::vector<std::thread> threads;

            // Start heathen and prude threads
            for (int i = 0; i < 5; ++i)
            {
                threads.emplace_back(execute_heathen);
                threads.emplace_back(execute_prude);
            }

            for (auto& thread : threads)
            {
                thread.join();
            }
        }
    }
}

#endif //SEMAPHORE_EXAMPLES_CPP_NOT_SO_CLASSICAL_PROBLEMS_H
