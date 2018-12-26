#include "Minion.h"
#include "Handler/AHubRequestHandler.h"
#include "Handler/MonitorRequestHandler.h"

Minion::Minion()
{
    std::cout << "Minion created\n";
}

Minion::Minion(const Minion &m) : name{m.name} {
    // std::cout << "Minion created with the name " << Minion::name << " - copy constructor \n";
}

Minion::Minion(std::string n) : name{n} {
    // std::cout << "Minion created with the name " << Minion::name << " - string constructor \n";
}

int main(int argc, char *argv[]) {
    std::cout << "Will try to create Minion\n";

    std::string s = "Minion I";
    Minion minion = Minion(s);

    AHubRequestHandler ahubRequestHandler = AHubRequestHandler(minion);
    std::thread ahubRequestHandlerThread(AHubRequestHandler::startAHubRequestHandler, ahubRequestHandler);

    MonitorRequestHandler monitorRequestHandler = MonitorRequestHandler(minion);
    std::thread monitorRequestHandlerThread(MonitorRequestHandler::startMonitorRequestHandler, monitorRequestHandler);

    ahubRequestHandlerThread.join();
    monitorRequestHandlerThread.join();

    std::cout << "Bubye\n";

    return 0;
}