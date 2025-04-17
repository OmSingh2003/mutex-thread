# Mutex Thread Synchronization Demo

This project demonstrates mutual exclusion using threads in C++. It shows how multiple threads can safely access and modify a shared resource using mutex synchronization.

## Overview

The program creates multiple threads that attempt to access and modify a shared counter. Each thread must enter a critical section to modify the counter, and mutex synchronization ensures that only one thread can be in the critical section at a time.

### Key Features

- Mutex-based thread synchronization
- Critical section protection
- Shared resource (counter) access control
- Real-time thread state visualization
- Multiple concurrent threads
 
## Requirements

- C++17 or later
- CMake 3.10 or later
- A C++ compiler with thread support

## Building the Project

```bash
# Create a build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build the project
make
```

## Running the Program

After building, run the executable:

```bash
./mutex_demo
```

## Program Output

The program will show:
- Threads waiting to enter the critical section
- Threads entering and exiting the critical section
- Counter value updates
- Final counter value

Example output:
```
Thread 0 waiting
Thread 1 waiting
Thread 0 entered critical section
Thread 0 incremented counter to 1
Thread 0 exiting critical section
...
Final counter value: 15
```

## Understanding the Code

### Key Components

1. **Mutex Lock**
   ```cpp
   std::mutex mutex;
   std::lock_guard<std::mutex> lock(mutex);
   ```
   - Ensures mutual exclusion in critical section
   - Automatically releases lock when leaving scope

2. **Thread Function**
   ```cpp
   void thread_function(int thread_id) {
       // Each thread attempts to increment counter 3 times
   }
   ```

3. **Critical Section**
   - Protected by mutex
   - Only one thread can access at a time
   - Contains shared resource modification

### Thread Synchronization

The program demonstrates:
- How threads compete for access to critical section
- Safe modification of shared resources
- Prevention of race conditions
- Proper thread cleanup and joining

## License

This project is open source and available under the MIT License.

## Contributing

Feel free to submit issues and enhancement requests! 
