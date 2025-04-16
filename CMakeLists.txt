cmake_minimum_required(VERSION 3.10)
project(mutex_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)

# Add executable
add_executable(mutex_demo mutex.cpp)

# Link libraries
target_link_libraries(mutex_demo PRIVATE Threads::Threads) 