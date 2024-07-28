//
// Created by ozgur on 7/21/2024.
//

#ifndef SEMAPHORE_EXAMPLES_CPP_BARRIER_H
#define SEMAPHORE_EXAMPLES_CPP_BARRIER_H

#include <semaphore>

class Barrier {
public:
    explicit Barrier(int _n);
    void phase1();
    void phase2();
    void wait();
private:
    int n;
    int count;
    [[maybe_unused]] std::binary_semaphore mutex;
    [[maybe_unused]] std::binary_semaphore turnstile;
    [[maybe_unused]] std::binary_semaphore turnstile2;
};

#endif //SEMAPHORE_EXAMPLES_CPP_BARRIER_H
