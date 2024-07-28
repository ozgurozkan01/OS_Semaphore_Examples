#include "introduction.h"
#include "basic_sycnhronization_patterns.h"
#include "classical_synchronization_problems.h"

using namespace introduction;
using namespace basic_synchronization_patterns;
using namespace classical_synchronization_problems;

int main()
{
/*
    // INTRODUCTION
     serialization::run();
     non_determinism::run();
     concurrent_writes::run();
     concurrent_updates::run();

    // BASIC SYNCH PATTERNS
     signaling::run();
     rendezvous::run();
     rendezvous_deadlock::run();
     mutex::run();
     multiplex::run();
     barrier_deadlock::run();
     barrier_solution::run();
     barrier_deadlock_2::run();
     reusable_barrier_deadlock::run();
     reusable_barrier_deadlock_2::run();
     reusable_barrier_solution::run();
     preloaded_turnstile::run();
     barrier_object::run();
     leader_and_follower_queue::run();
 */

    // CLASSICAL SYNCH PROBLEMS
    // producer_consumer_problem_infinite::run();
    // producer_consumer_problem_finite::run();
    // readers_and_writers_problem::run();
    // readers_and_writers_problem::run();
    no_starving_mutex::run();

    return 0;
}