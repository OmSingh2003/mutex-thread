#include "thread_manager.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>

using namespace std::chrono_literals;

// Helper function to get current timestamp
std::string get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto now_time = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    std::stringstream ss;
    ss << std::put_time(std::localtime(&now_time), "%H:%M:%S") << "." << std::setfill('0') << std::setw(3) << now_ms.count();
    return ss.str();
}

ThreadManager::ThreadManager() = default;

ThreadManager::~ThreadManager() {
    stop_threads();
}

void ThreadManager::start_threads(int count) {
    std::lock_guard<std::mutex> lock(thread_mutex);
    stop_threads();
    
    shared_counter = 0;
    stop_requested = false;
    
    threads.clear();
    threads.resize(count);
    
    for (int i = 0; i < count; ++i) {
        threads[i].state = "waiting";
        threads[i].message = "Waiting to enter critical section";
        threads[i].running = true;
        threads[i].thread = std::thread(&ThreadManager::thread_function, this, i);
    }
}

void ThreadManager::stop_threads() {
    {
        std::lock_guard<std::mutex> lock(thread_mutex);
        if (threads.empty()) return;
        
        stop_requested = true;
    }
    
    cv.notify_all();
    
    for (auto& thread_info : threads) {
        if (thread_info.thread.joinable()) {
            thread_info.thread.join();
        }
    }
    
    threads.clear();
}

void ThreadManager::thread_function(int thread_id) {
    while (!stop_requested) {
        // Try to enter critical section
        {
            std::lock_guard<std::mutex> state_lock(state_mutex);
            threads[thread_id].state = "waiting";
            threads[thread_id].message = "Waiting to enter critical section";
        }
        
        {
            std::unique_lock<std::mutex> lock(thread_mutex);
            
            if (stop_requested) break;
            
            // Enter critical section
            {
                std::lock_guard<std::mutex> state_lock(state_mutex);
                threads[thread_id].state = "critical";
                threads[thread_id].message = "Entered critical section";
            }
            
            // Update shared resource
            int current = shared_counter.load();
            std::this_thread::sleep_for(100ms); // Simulate work
            shared_counter.store(current + 1);
            
            {
                std::lock_guard<std::mutex> state_lock(state_mutex);
                threads[thread_id].message = "Updated counter to " + std::to_string(current + 1);
            }
            
            // Exit critical section
            {
                std::lock_guard<std::mutex> state_lock(state_mutex);
                threads[thread_id].state = "non-critical";
                threads[thread_id].message = "Exited critical section";
            }
        }
        
        // Non-critical section
        std::this_thread::sleep_for(200ms);
    }
    
    // Thread is stopping
    std::lock_guard<std::mutex> state_lock(state_mutex);
    threads[thread_id].state = "stopped";
    threads[thread_id].message = "Thread stopped";
    threads[thread_id].running = false;
}

int ThreadManager::get_shared_resource() const {
    return shared_counter.load();
}

size_t ThreadManager::get_thread_count() const {
    std::lock_guard<std::mutex> lock(thread_mutex);
    return threads.size();
}

std::string ThreadManager::get_thread_state(size_t thread_id) const {
    std::lock_guard<std::mutex> lock(state_mutex);
    if (thread_id < threads.size()) {
        return threads[thread_id].state;
    }
    return "unknown";
}

std::string ThreadManager::get_thread_message(size_t thread_id) const {
    std::lock_guard<std::mutex> lock(state_mutex);
    if (thread_id < threads.size()) {
        return threads[thread_id].message;
    }
    return "Unknown thread";
} 