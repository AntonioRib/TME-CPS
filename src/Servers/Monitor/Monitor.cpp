#include "Monitor.h"
#include "Handler/AHubRequestHandler.h"
#include "Handler/AuditorRequestHandler.h"
#include "Handler/DeveloperRequestHandler.h"
#include "Handler/MinionRequestHandler.h"

Monitor::Monitor(std::string n) : name{n} {
    std::cout << "Monitor created with the name " << Monitor::name << "\n";
}

Monitor::Monitor() {
    std::cout << "Monitor created\n";
}

int main(int argc, char* argv[]) {
    std::cout << "Will try to create Monitor\n";

    std::string s = "Monitor I";
    Monitor *monitor = new Monitor(s);

    std::thread aHubHandler = std::thread([monitor] { AHubRequestHandler(monitor->name); });
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