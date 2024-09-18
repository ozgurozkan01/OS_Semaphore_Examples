cmake_minimum_required(VERSION 3.28)
project(Semaphore_Examples_CPP)

set(CMAKE_CXX_STANDARD 20)

add_executable(Semaphore_Examples_CPP main.cpp
        src/Barrier.cpp
        include/Barrier.h
        include/introduction.h
        include/basic_sycnhronization_patterns.h
        include/classical_synchronization_problems.h
        include/less_classical_synchronization_problems.h
        include/not_so_classical_problems.h
        include/single_linked_list.h
        include/not_remotely_classical_problems.h
)
