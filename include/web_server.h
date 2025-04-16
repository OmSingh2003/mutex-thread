#pragma once

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>
#include "thread_manager.h"

class WebServer {
public:
    explicit WebServer(ThreadManager& thread_manager);
    void start(int port);
    void stop();

private:
    ThreadManager& thread_manager;
    httplib::Server server;
    bool running{true};
}; 