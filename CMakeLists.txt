cmake_minimum_required(VERSION 3.10)

project(thread_pool)

add_compile_options(-Wall -std=c++20)

add_executable(demo test.cpp src/thread_pool.cpp src/logger.cpp src/utils.cpp)