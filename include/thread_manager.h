#pragma once

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <condition_variable>

class ThreadManager {
public:
    ThreadManager();
    ~ThreadManager();

    void start_threads(int count);
    void stop_threads();
    int get_shared_resource() const;
    size_t get_thread_count() const;
    std::string get_thread_state(size_t thread_id) const;
    std::string get_thread_message(size_t thread_id) const;

private:
    void thread_function(int thread_id);
    
    struct ThreadInfo {
        std::string state;
        std::string message;
        std::thread thread;
        bool running;
    };

    std::vector<ThreadInfo> threads;
    mutable std::mutex thread_mutex;
    mutable std::mutex state_mutex;
    std::atomic<int> shared_counter{0};
    std::atomic<bool> stop_requested{false};
    std::condition_variable cv;
}; 