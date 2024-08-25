//
// Created by ozgur on 8/22/2024.
//

#ifndef SEMAPHORE_EXAMPLES_CPP_LESS_CLASSICAL_SYNCHRONIZATION_PROBLEMS_H
#define SEMAPHORE_EXAMPLES_CPP_LESS_CLASSICAL_SYNCHRONIZATION_PROBLEMS_H

#include <thread>
#include <mutex>

namespace less_classical_synchronization_problems
{
    namespace dining_savages_problem
    {
        // IN THIS EXAMPLE SCOREBOARD PATTERN IS USED !!

        /*
         - LOGIC OF RUNNING !!
         The code ensures that savages can only eat when there are servings in the pot.
         If the pot is empty, the first savage to discover this will signal the cook to refill it,
         ensuring synchronization between the savages and the cook.

         - CODE OUTPUT !!
            Savage got the serving from pot..
            3. savage is eating..
            Savage got the serving from pot..
            5. savage is eating..
            Savage got the serving from pot..
            4. savage is eating..
            Savage got the serving from pot..
            3. savage is eating..
            Savage got the serving from pot..
            5. savage is eating..
            Servings is put into pot by cook!!

            The output will repeat as savages continue to eat and the cook continues to refill the pot whenever it's empty.
            The specific thread IDs and order of actions may vary because the threads run concurrently.
         */

        constexpr int savage_number = 3;
        constexpr int M = 5; // stands for serving amount
        int servings = 0;
        std::mutex mutex;
        std::binary_semaphore emptyPot(0); // It is binary semaphore, because we deal with just it is empty or not
        std::binary_semaphore fullPot(0); // It is binary semaphore, because we deal with just it is empty or not

        void execute_savage()
        {
            while(true)
            {
                mutex.lock(); // if a thread gets in, others have to wait till it is available.
                if (servings == 0) // No portions to eat means pot is empty
                {
                    emptyPot.release(); // pot is empty so deal with implementations when pot is empty
                    fullPot.acquire(); // blocked to stop transactions that take place while the pot is full

                    /*
                     Why savage is reset the servings ? if u r saying, I could not understand. Shouldn't the cook do it ?
                     Because at the beginning of this problem,

                     "When a savage wants to eat, he helps himself from the pot, unless it is empty. If the pot is
                     empty, the savage wakes up the cook and then waits until the cook
                     has refilled the pot."  is said. U r totally right, but there are a couple of reason of this.

                     1- Access Consistency: Wild accesses the servings variable only when the mutex is received, which ensures that data is updated safely and prevents data races.

                     2- Deadlock Risk Prevention: If the cook updated the servings variable,the cook would update the servings value after filling the pot and send fullPot. signal().
                     In this case, the wild holding the mutex could wait for the cook to access the mutex, which could cause a deadlock.

                     */
                    servings = M; // set the portions as full

                }
                servings--; // decrease the pot
                std::cout << "Savage got the serving from pot..\n"; // get_serving_from_pot();
                mutex.unlock();
                std::cout << std::this_thread::get_id() << ". savage is eating..\n"; // eat();
            }
        }

        void execute_cook()
        {
            while (true)
            {
                emptyPot.acquire(); // if pot is empty
                std::cout << "Servings is put into pot by cook!!\n"; // put_servings_in_pot(M);
                fullPot.release(); // arrange pot is full
            }
        }

        void run()
        {
            std::vector<std::thread> savages;

            std::thread cook(execute_cook);

            for (int i = 0; i < savage_number; ++i)
            {
                 savages.push_back(std::thread(execute_savage));
            }

            if (cook.joinable()) { cook.join(); }

            for (int i = 0; i < savage_number; ++i)
            {
                if (savages.at(i).joinable()) { savages.at(i).join(); }
            }
        }
    }

    namespace the_barbershop_problem
    {
        /*
         - LOGIC OF RUNNING !!
         - CODE OUTPUT !!
         */

        constexpr int n = 4; // total number of customers, 3 in waiting room, 1 in chair
        int customer_counter = 0; // number of customers in the shop
        std::mutex mutex;
        std::binary_semaphore barber(0); // there is just one barber
        std::binary_semaphore customer(0); // customer who is shaving
        std::binary_semaphore barber_done(0); // signals to the customer when barber is done.
        std::binary_semaphore customer_done(0); // signals to the barber when customer is done.

        void execute_customer()
        {
            while (true)
            {
                mutex.lock();
                if (customer_counter == n)
                {
                    mutex.unlock();
                    std::cout << std::this_thread::get_id() << ". customer balked the new customer..\n"; // balk();
                    return;
                }
                customer_counter++;
                mutex.unlock();

                customer.release();
                barber.acquire();

                std::cout << std::this_thread::get_id() << ". customer got hair cut..\n"; // getHairCut();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                customer_done.release();
                barber_done.acquire();

                mutex.lock();
                customer_counter--;
                mutex.unlock();
            }
        }

        void execute_barber()
        {
            while (true)
            {
                customer.acquire();
                barber.release();
                std::cout << "Barber cut the customers hair...\n"; // cutHair();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                customer_done.acquire();
                barber_done.release();
            }
        }

        void run()
        {
            std::vector<std::thread> customers_thread;
            std::thread barber_thread(execute_barber);

            for (int i = 0; i < n+2; ++i) // More than n customers
            {
                customers_thread.push_back(std::thread(execute_customer));
            }

            for (int i = 0; i < n; ++i)
            {
                if (customers_thread.at(i).joinable()) { customers_thread.at(i).join(); }
            }

            if (barber_thread.joinable()) { barber_thread.join(); }
        }
    }
}

#endif //SEMAPHORE_EXAMPLES_CPP_LESS_CLASSICAL_SYNCHRONIZATION_PROBLEMS_H
