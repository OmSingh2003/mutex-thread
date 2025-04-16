#include "web_server.h"
#include "thread_manager.h"
#include <iostream>
#include <csignal>
#include <atomic>

std::atomic<bool> running{true};

void signal_handler(int signal) {
    std::cout << "\nReceived signal " << signal << ". Shutting down gracefully..." << std::endl;
    running = false;
}

int main() {
    try {
        // Set up signal handlers
        std::signal(SIGINT, signal_handler);
        std::signal(SIGTERM, signal_handler);

        ThreadManager thread_manager;
        WebServer server(thread_manager);
        
        std::cout << "Starting server on http://localhost:8080" << std::endl;
        std::cout << "Press Ctrl+C to stop the server" << std::endl;
        
        // Start server in a separate thread
        std::thread server_thread([&server]() {
            server.start(8080);
        });

        // Wait for shutdown signal
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Clean shutdown
        server.stop();
        thread_manager.stop_threads();
        
        if (server_thread.joinable()) {
            server_thread.join();
        }

        std::cout << "Server shutdown complete" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
} 