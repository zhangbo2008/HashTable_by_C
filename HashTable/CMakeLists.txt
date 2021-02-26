cmake_minimum_required(VERSION 3.15)
project(HashTable C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(HashTable
        HashTable.c
        HashTable.h
        main.c)
