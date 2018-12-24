#include "Monitor.h"
#include "Handler/AHubRequestHandler.h"
#include "Handler/AuditorRequestHandler.h"
#include "Handler/DeveloperRequestHandler.h"
#include "Handler/MinionRequestHandler.h"

Monitor::Monitor(const Monitor &m) : name{m.name} {
    std::cout << "Monitor created with the name " << Monitor::name << " - copy constructor \n";
}

Monitor::Monitor(std::string n) : name{n} {
    std::cout << "Monitor created with the name " << Monitor::name << " - string constructor \n";
}

Monitor::Monitor() {
    std::cout << "Monitor created\n";
}

int main(int argc, char* argv[]) {
    std::cout << "Will try to create Monitor\n";

    std::string s = "Monitor I";
    Monitor monitor = Monitor(s);

    AHubRequestHandler a = AHubRequestHandler(monitor); 
    std::thread aHubHandler(AHubRequestHandler::startAHubRequestHandler, a);
    // std::thread aHubHandler(a.startAHubRequestHandler); //=  //std::thread([&monitor] { AHubRequestHandler(monitor); });
    //     std::thread auditorHandler = std::thread([monitor] { AuditorRequestHandler(monitor); });
    // std::thread developerHandler = std::thread([monitor] { DeveloperRequestHandler(monitor); });
    // std::thread minionHandler = std::thread([monitor] { MinionRequestHandler(monitor); });

    aHubHandler.join();
    // auditorHandler.join();
    // developerHandler.join();
    // minionHandler.join();

    std::cout << "Bubye\n";

    return 0;
}