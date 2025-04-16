#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

std::mutex mutex;
int shared_counter = 0;

void thread_function(int thread_id) {
    for (int i = 0; i < 3; i++) {
        // Try to enter critical section
        std::cout << "Thread " << thread_id << " waiting\n";
        
        {
            std::lock_guard<std::mutex> lock(mutex);
            
            // Critical section
            std::cout << "Thread " << thread_id << " entered critical section\n";
            int current = shared_counter;
            shared_counter = current + 1;
            std::cout << "Thread " << thread_id << " incremented counter to " << shared_counter << "\n";
            std::cout << "Thread " << thread_id << " exiting critical section\n";
        }
    }
}

int main() {
    std::vector<std::thread> threads;
    
    // Create 5 threads
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(thread_function, i);
    }
    
    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }
    
    std::cout << "Final counter value: " << shared_counter << "\n";
    return 0;
} 