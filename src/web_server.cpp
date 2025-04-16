#include "web_server.h"
#include <sstream>
#include <json/json.h>
#include <csignal>

WebServer::WebServer(ThreadManager& thread_manager) : thread_manager(thread_manager) {
    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content(
            "<!DOCTYPE html>\n"
            "<html>\n"
            "<head>\n"
            "    <title>Thread Synchronization Demo</title>\n"
            "    <style>\n"
            "        body { font-family: Arial, sans-serif; margin: 20px; background: #f5f5f5; }\n"
            "        .container { max-width: 1000px; margin: 0 auto; }\n"
            "        .controls { margin: 20px 0; padding: 20px; background: white; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n"
            "        .status { margin-top: 20px; padding: 20px; background: white; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n"
            "        .thread-container { display: flex; flex-wrap: wrap; gap: 20px; margin-top: 20px; }\n"
            "        .thread { flex: 1; min-width: 300px; padding: 20px; background: white; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }\n"
            "        .thread h3 { margin-top: 0; color: #333; }\n"
            "        .thread-status { margin-top: 10px; padding: 10px; border-radius: 4px; }\n"
            "        .waiting { background: #ffd700; color: #333; }\n"
            "        .critical { background: #ff4444; color: white; }\n"
            "        .non-critical { background: #4CAF50; color: white; }\n"
            "        button { padding: 10px 20px; margin-right: 10px; border: none; border-radius: 4px; cursor: pointer; font-size: 16px; }\n"
            "        .start-btn { background: #4CAF50; color: white; }\n"
            "        .stop-btn { background: #f44336; color: white; }\n"
            "        input[type='number'] { padding: 8px; border: 1px solid #ddd; border-radius: 4px; width: 60px; }\n"
            "        .log-container { margin-top: 20px; padding: 20px; background: white; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); max-height: 300px; overflow-y: auto; }\n"
            "        .log-entry { padding: 5px 0; border-bottom: 1px solid #eee; }\n"
            "        .timestamp { color: #666; margin-right: 10px; }\n"
            "    </style>\n"
            "</head>\n"
            "<body>\n"
            "    <div class=\"container\">\n"
            "        <h1>Thread Synchronization Demo</h1>\n"
            "        <div class=\"controls\">\n"
            "            <button class=\"start-btn\" onclick=\"startThreads()\">Start Threads</button>\n"
            "            <button class=\"stop-btn\" onclick=\"stopThreads()\">Stop Threads</button>\n"
            "            <input type=\"number\" id=\"threadCount\" value=\"3\" min=\"1\" max=\"10\">\n"
            "        </div>\n"
            "        <div class=\"status\">\n"
            "            <h2>Shared Resource Value: <span id=\"resourceValue\">0</span></h2>\n"
            "        </div>\n"
            "        <div class=\"thread-container\" id=\"threadContainer\">\n"
            "            <!-- Thread status cards will be added here -->\n"
            "        </div>\n"
            "        <div class=\"log-container\" id=\"logContainer\">\n"
            "            <!-- Log entries will be added here -->\n"
            "        </div>\n"
            "    </div>\n"
            "    <script>\n"
            "        let updateInterval;\n"
            "        let threads = [];\n"
            "\n"
            "        function createThreadCard(threadId) {\n"
            "            const threadCard = document.createElement('div');\n"
            "            threadCard.className = 'thread';\n"
            "            threadCard.id = `thread-${threadId}`;\n"
            "            threadCard.innerHTML = `\n"
            "                <h3>Thread ${threadId}</h3>\n"
            "                <div class=\"thread-status waiting\">Waiting to enter critical section</div>\n"
            "            `;\n"
            "            document.getElementById('threadContainer').appendChild(threadCard);\n"
            "            return threadCard;\n"
            "        }\n"
            "\n"
            "        function addLogEntry(message) {\n"
            "            const logEntry = document.createElement('div');\n"
            "            logEntry.className = 'log-entry';\n"
            "            const timestamp = new Date().toLocaleTimeString();\n"
            "            logEntry.innerHTML = `<span class=\"timestamp\">${timestamp}</span>${message}`;\n"
            "            const logContainer = document.getElementById('logContainer');\n"
            "            logContainer.insertBefore(logEntry, logContainer.firstChild);\n"
            "        }\n"
            "\n"
            "        function updateStatus() {\n"
            "            fetch('/status')\n"
            "                .then(response => response.json())\n"
            "                .then(data => {\n"
            "                    document.getElementById('resourceValue').textContent = data.value;\n"
            "                    if (data.threads) {\n"
            "                        data.threads.forEach(thread => {\n"
            "                            const threadCard = document.getElementById(`thread-${thread.id}`) || createThreadCard(thread.id);\n"
            "                            const statusDiv = threadCard.querySelector('.thread-status');\n"
            "                            statusDiv.className = 'thread-status ' + thread.state;\n"
            "                            statusDiv.textContent = thread.message;\n"
            "                        });\n"
            "                    }\n"
            "                });\n"
            "        }\n"
            "\n"
            "        function startThreads() {\n"
            "            const count = document.getElementById('threadCount').value;\n"
            "            document.getElementById('threadContainer').innerHTML = '';\n"
            "            document.getElementById('logContainer').innerHTML = '';\n"
            "            fetch('/start/' + count);\n"
            "            updateInterval = setInterval(updateStatus, 100);\n"
            "        }\n"
            "\n"
            "        function stopThreads() {\n"
            "            clearInterval(updateInterval);\n"
            "            fetch('/stop');\n"
            "        }\n"
            "    </script>\n"
            "</body>\n"
            "</html>",
            "text/html"
        );
    });

    server.Get("/status", [&](const httplib::Request&, httplib::Response& res) {
        Json::Value response;
        response["value"] = thread_manager.get_shared_resource();
        
        // Get thread states
        Json::Value threads(Json::arrayValue);
        for (size_t i = 0; i < thread_manager.get_thread_count(); ++i) {
            Json::Value thread;
            thread["id"] = static_cast<int>(i);
            thread["state"] = thread_manager.get_thread_state(i);
            thread["message"] = thread_manager.get_thread_message(i);
            threads.append(thread);
        }
        response["threads"] = threads;
        
        Json::StreamWriterBuilder builder;
        res.set_content(Json::writeString(builder, response), "application/json");
    });

    server.Get("/start/(\\d+)", [&](const httplib::Request& req, httplib::Response&) {
        int num_threads = std::stoi(req.matches[1]);
        thread_manager.start_threads(num_threads);
    });

    server.Get("/stop", [&](const httplib::Request&, httplib::Response&) {
        thread_manager.stop_threads();
    });

    // Set up signal handler for graceful shutdown
    std::signal(SIGINT, [](int) {
        std::cout << "\nShutting down server..." << std::endl;
        exit(0);
    });
}

void WebServer::start(int port) {
    server.listen("0.0.0.0", port);
}

void WebServer::stop() {
    if (running) {
        running = false;
        server.stop();
    }
} 