#include "Monitor.h"
#include "Handler/AHubRequestHandler.h"

Monitor::Monitor() {
    std::cout << "Monitor created\n";
    std::cout << "Try to create AHub Handler created\n";
    std::thread aHubHandler = this->startAHubHandler();
    aHubHandler.join();
}

std::thread Monitor::startAHubHandler() {
    return std::thread([this] { AHubRequestHandler(); });
}

int main(int argc, char* argv[]) {
    std::cout << "Will try to create Monitor\n";

    Monitor* monitor;
    monitor = new Monitor();

    std::cout << "Bubye\n";

    return 0;
}