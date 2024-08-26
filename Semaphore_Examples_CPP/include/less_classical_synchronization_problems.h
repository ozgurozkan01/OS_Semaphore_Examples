//
// Created by ozgur on 8/22/2024.
//

#ifndef SEMAPHORE_EXAMPLES_CPP_LESS_CLASSICAL_SYNCHRONIZATION_PROBLEMS_H
#define SEMAPHORE_EXAMPLES_CPP_LESS_CLASSICAL_SYNCHRONIZATION_PROBLEMS_H

#include <thread>
#include <mutex>
#include <queue>

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
            • Customer threads should invoke a function named getHairCut.
            • If a customer thread arrives when the shop is full, it can invoke balk, which does not return.
            • The barber thread should invoke cutHair.
            • When the barber invokes cutHair there should be exactly one thread invoking getHairCut concurrently.

         - CODE OUTPUT !!
          while code is running, the output can be seen correct. But it is not. Let's check it.

            - Code piece from output :

            Barber cut the customers hair...
            6. customer got hair cut..
            Barber cut the customers hair...
            6. customer got hair cut..
            Barber cut the customers hair...
            6. customer got hair cut..

           As u see, no guarantee the barber is gonna cut the customers' hairs in order. So that is the problem.
           Up to n customers can pass the turnstile, signal customer and wait on barber.
           When the barber signal barber, any of the customers might proceed.

        - SOLUTION !!
            FIFO BARBERSHOP PROBLEM...
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
                    std::cout << std::this_thread::get_id() << ". customer is balked..\n"; // balk();
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

    namespace the_fifo_barbershop_problem
    {
        /*
         - CODE OUTPUT !!
            In previous solution, there is no guarantee the barber cut the customers' hair in order.
            In this solution, we use the queue and every thread is using its own semaphore.
            After its shaving is done, we pop the its semaphore from the queue. When its turn comes,
            then push its semaphore again into the queue. And that solves the synch problem.

            - Code piece of output !!
            Barber cut the customers hair...
            7. customer got hair cut..
            Barber cut the customers hair...
            3. customer got hair cut..
            Barber cut the customers hair...
            6. customer got hair cut..
            Barber cut the customers hair...
            5. customer got hair cut..
            Barber cut the customers hair...
            7. customer got hair cut..
            Barber cut the customers hair...
            3. customer got hair cut..
            Barber cut the customers hair...
            6. customer got hair cut..
         */

        struct sem
        {
            std::binary_semaphore own_sem = std::binary_semaphore(0);
        };

        constexpr int n = 4; // total number of customers, 3 in waiting room, 1 in chair
        int customer_counter = 0; // number of customers in the shop
        std::mutex mutex;
        // std::binary_semaphore barber(0); // there is just one barber
        std::binary_semaphore customer(0); // customer who is shaving
        std::binary_semaphore barber_done(0); // signals to the customer when barber is done.
        std::binary_semaphore customer_done(0); // signals to the barber when customer is done.
        std::queue<std::shared_ptr<sem>> customers_fifo; // solver the synch problem.

        void execute_customer()
        {
            while (true)
            {
                auto s = std::make_shared<sem>(); // Create a shared pointer to sem

                mutex.lock();
                if (customer_counter == n)
                {
                    mutex.unlock();
                    std::cout << std::this_thread::get_id() << ". customer is balked..\n"; // balk();
                    return;
                }
                customer_counter++;
                customers_fifo.push(s);
                mutex.unlock();

                customer.release();
                s->own_sem.acquire();

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
                mutex.lock();

                auto s = customers_fifo.front();
                customers_fifo.pop();

                mutex.unlock();

                s->own_sem.release();

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

    namespace hilzers_barbershop_problem
    {
        /*
         - LOGIC OF RUNNING !!
            • Customers invoke the following functions in order: enterShop, sitOnSofa, getHairCut, pay.
            • Barbers invoke cutHair and acceptPayment.
            • Customers cannot invoke enterShop if the shop is at capacity.
            • If the sofa is full, an arriving customer cannot invoke sitOnSofa.
            • When a customer invokes getHairCut there should be a corresponding barber executing cutHair concurrently, and vice versa.
            • It should be possible for up to three customers to execute getHairCut concurrently, and up to three barbers to execute cutHair concurrently.
            • The customer has to pay before the barber can acceptPayment.
            • The barber must acceptPayment before the customer can exit.

         - CODE OUTPUT !!
            All threads run synchronously with each other and operations happen simultaneously.
            Therefore, after a customer1 walks in, customer2 can sit on the barber's chair
            before customer1 can sit on the table. Therefore the output can look confusing
            because there are too many customers in the code.

            5. customer entered the barbershop..
            7. customer entered the barbershop..
            8. customer entered the barbershop..
            10. customer entered the barbershop..
            6. customer entered the barbershop..
            3. customer entered the barbershop..
            11. customer entered the barbershop..
            14. customer entered the barbershop..
            16. customer entered the barbershop..
            17. customer entered the barbershop..
            4. customer entered the barbershop..
            19. customer entered the barbershop..
            23. customer entered the barbershop..
            21. customer is balked..
            13. customer entered the barbershop..
            15. customer entered the barbershop..
            20. customer entered the barbershop..
            12. customer entered the barbershop..
            9. customer entered the barbershop..
            22. customer entered the barbershop..
            18. customer entered the barbershop..

            24. customer is balked..
            5. customer sit on sofa..
            2. customer cut hair..
            5. customer sit in barber chair..
            5. customer paid..
            Barber accepted the payment
            7. customer sit on sofa..
            5. customer entered the barbershop..
            2. customer cut hair..
         */

        constexpr int n = 20; // capacity
        int customer_counter = 0;
        std::mutex mutex;
        std::counting_semaphore<4> sofa(4); // Sofa capacity counter
        std::binary_semaphore customer1(0);
        std::binary_semaphore customer2(0);
        std::binary_semaphore barber(0);
        std::binary_semaphore payment(0);
        std::binary_semaphore receipt(0);

        std::queue<std::shared_ptr<std::binary_semaphore>> queue1;
        std::queue<std::shared_ptr<std::binary_semaphore>> queue2;

        void execute_customer()
        {
            while (true)
            {
                auto s1 = std::make_shared<std::binary_semaphore>(0); // Create a shared pointer to sem
                auto s2 = std::make_shared<std::binary_semaphore>(0); // Create a shared pointer to sem

                mutex.lock();
                if (customer_counter == n)
                {
                    mutex.unlock();
                    std::cout << std::this_thread::get_id() << ". customer is balked..\n"; // balk();
                    return;
                }
                customer_counter++;
                queue1.push(s1);
                mutex.unlock();

                std::cout << std::this_thread::get_id() << ". customer entered the barbershop..\n"; // enterShop();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                customer1.release();
                s1->acquire();

                sofa.acquire();

                std::cout << std::this_thread::get_id() << ". customer sit on sofa..\n"; // sitOnSofa();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                s1->release();
                mutex.lock();
                queue2.push(s2);
                mutex.unlock();
                customer2.release();
                s2->acquire();
                sofa.release();

                std::cout << std::this_thread::get_id() << ". customer sit in barber chair..\n"; // sitInBarberChair();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                std::cout << std::this_thread::get_id() << ". customer paid..\n"; // pay();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                payment.release();
                receipt.acquire();

                mutex.lock();
                customer_counter--;
                mutex.unlock();
            }
        }

        void execute_barber() {
            while (true) {
                customer1.acquire();
                mutex.lock();
                auto s = queue1.front();
                queue1.pop();
                s->release();
                s->acquire();
                mutex.unlock();
                s->release();

                customer2.acquire();
                mutex.lock();
                s = queue2.front();
                queue2.pop();
                mutex.unlock();
                s->release();

                barber.release();

                std::cout << std::this_thread::get_id() << ". customer cut hair..\n"; // cutHair();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                payment.acquire();

                std::cout << "Barber accepted the payment\n" ; // acceptPayment();
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));

                receipt.release();
            }
        }

        void run()
        {
            std::vector<std::thread> customers_thread;
            std::vector<std::thread> barbers_thread;

            for (int i = 0; i < n+2; ++i) // More than n customers
            {
                customers_thread.push_back(std::thread(execute_customer));
            }

            for (int i = 0; i < 3; ++i)
            {
                barbers_thread.push_back(std::thread(execute_barber));
            }

            for (int i = 0; i < n+2; ++i)
            {
                if (customers_thread.at(i).joinable()) { customers_thread.at(i).join(); }
            }

            for (int i = 0; i < 3; ++i)
            {
                if (barbers_thread.at(i).joinable()) { barbers_thread.at(i).join(); }
            }
        }
    }
}

#endif //SEMAPHORE_EXAMPLES_CPP_LESS_CLASSICAL_SYNCHRONIZATION_PROBLEMS_H
