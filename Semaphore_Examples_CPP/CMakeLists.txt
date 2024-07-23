cmake_minimum_required(VERSION 3.28)
project(Semaphore_Examples_CPP)

set(CMAKE_CXX_STANDARD 20)

add_executable(Semaphore_Examples_CPP main.cpp
        Barrier.cpp
        Barrier.h
        introduction.h
        basic_sycnhronization_patterns.h
        classical_synchronization_problems.h
)
