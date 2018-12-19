#include "Monitor.h"


Monitor::Monitor() {
    std::cout << "Monitor created\n";
    std::cout << "Try to create AHub Handler created\n";
}

int main(int argc, char* argv[]) {
    std::cout << "Will try to create Monitor\n";

    Monitor monitor = Monitor();

    std::thread aHubHandler = std::thread([monitor] { AHubRequestHandler(monitor); });
    std::thread auditorHandler = std::thread([monitor] { AuditorRequestHandler(monitor); });
    std::thread developerHandler = std::thread([monitor] { DeveloperRequestHandler(monitor); });
    std::thread minionHandler = std::thread([monitor] { MinionRequestHandler(monitor); });




    aHubHandler.join();
    auditorHandler.join();
    developerHandler.join();
    minionHandler.join();

    std::cout << "Bubye\n";

    return 0;
}