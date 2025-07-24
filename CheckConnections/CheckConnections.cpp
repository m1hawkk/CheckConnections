#include <iostream>
#include <string>
#include <array>
#include <memory>
#include <thread>
#include <chrono>
#include <windows.h>
#include <ctime>
#include <sstream>

std::string getTimeNow() {
    std::time_t t = std::time(nullptr);
    std::tm tm;
    localtime_s(&tm, &t);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(buf);
}

void checkPort(int port) {
    std::string cmd = "netstat -ano | find \":" + std::to_string(port) + "\"";
    std::array<char, 512> buf;

    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
    if (!pipe) return;

    std::cout << "[" << getTimeNow() << "] Port 端口 " << port << ":\n";

    bool hasData = false;
    while (fgets(buf.data(), buf.size(), pipe.get()) != nullptr) {
        std::string line = buf.data();
        if (line.find("TCP") != std::string::npos) {
            std::istringstream iss(line);
            std::string proto, local, remote, state, pid;
            iss >> proto >> local >> remote >> state >> pid;
            std::cout << "- IP: " << remote << " | PID: " << pid << " | " << state << "\n";
            hasData = true;
        }
    }

    if (!hasData) std::cout << "- No connection 无连接\n";
    std::cout << "\n";
}

int main() {
    SetConsoleOutputCP(CP_UTF8);

    while (true) {
        std::cout << "============================================\n";
        for (int port = 8080; port <= 8083; ++port)
            checkPort(port);

        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
}
