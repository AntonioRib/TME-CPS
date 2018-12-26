#include "Monitor.h"
#include "Handler/AHubRequestHandler.h"
#include "Handler/AuditorRequestHandler.h"
#include "Handler/DeveloperRequestHandler.h"
#include "Handler/MinionRequestHandler.h"

Monitor::Monitor(const Monitor& m) : name{m.name} {
    // std::cout << "Monitor created with the name " << Monitor::name << " - copy constructor \n";
}

Monitor::Monitor(std::string n) : name{n} {
    // std::cout << "Monitor created with the name " << Monitor::name << " - string constructor \n";
}

Monitor::Monitor() {
    std::cout << "Monitor created\n";
}

int main(int argc, char* argv[]) {
    std::cout << "Will try to create Monitor\n";

    std::string s = "Monitor I";
    Monitor monitor = Monitor(s);

    AHubRequestHandler ahubRequestHandler = AHubRequestHandler(monitor);
    std::thread ahubRequestHandlerThread(AHubRequestHandler::startAHubRequestHandler, ahubRequestHandler);

    AuditorRequestHandler auditorRequestHandler = AuditorRequestHandler(monitor);
    std::thread auditorRequestHandlerThread(AuditorRequestHandler::startAuditorRequestHandler, auditorRequestHandler);

    DeveloperRequestHandler developerRequestHandler = DeveloperRequestHandler(monitor);
    std::thread developerRequestHandlerThread(DeveloperRequestHandler::startDeveloperRequestHandler, developerRequestHandler);

    MinionRequestHandler minionRequestHandler = MinionRequestHandler(monitor);
    std::thread minionRequestHandlerThread(MinionRequestHandler::startMinionRequestHandler, minionRequestHandler);

    ahubRequestHandlerThread.join();
    auditorRequestHandlerThread.join();
    developerRequestHandlerThread.join();
    minionRequestHandlerThread.join();

    std::cout << "Bubye\n";

    return 0;
}