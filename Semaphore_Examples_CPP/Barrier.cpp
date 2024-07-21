//
// Created by ozgur on 7/21/2024.
//

#include "Barrier.h"

Barrier::Barrier(int _n) :
        n(_n),
        count(0),
        mutex(1),
        turnstile(0),
        turnstile2(0)
{}

void Barrier::phase1()
{
    mutex.acquire(); // wait , lock
    count++; // counts the thread which gets in
    if (count == n) { turnstile.release(n); } // Allowe to pass whole threads
    mutex.release();

    turnstile.acquire();
}

void Barrier::phase2()
{
    mutex.acquire(); // wait , lock
    count--; // counts the thread which gets out
    if (count == 0) { turnstile2.release(n); } // Allowe to pass whole threads
    mutex.release();

    turnstile2.acquire();
}

void Barrier::wait()
{
    phase1();
    phase2();
}