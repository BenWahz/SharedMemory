cmake_minimum_required(VERSION 3.15)
project(SharedMemory C)

set(CMAKE_C_STANDARD 99)

add_executable(SharedMemory procpal1.hmrice.c shmem-example.c procpal2.hmrice.c)