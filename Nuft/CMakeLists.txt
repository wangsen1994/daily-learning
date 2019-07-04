cmake_minimum_required(VERSION 3.14)
project(Nuft)

set(CMAKE_CXX_STANDARD 17)

add_executable(Nuft main.cpp Base/thread_pool.h Base/log.h Base/stiring_utils.h Base/lang_extend.h node.h)