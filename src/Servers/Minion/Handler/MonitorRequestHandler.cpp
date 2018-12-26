#include "MonitorRequestHandler.h"

MonitorRequestHandler::MonitorRequestHandler() {
    std::cout << "MonitorRequestHandler created\n";
}

MonitorRequestHandler::MonitorRequestHandler(Minion minion) : minion{minion} {
    std::cout << "MonitorRequestHandler created with the name " << minion.name << "\n";
}

void MonitorRequestHandler::startMonitorRequestHandler(MonitorRequestHandler monitorRequestHandler) {
    std::cout << "MonitorRequestHandler running with Minion with the name " << monitorRequestHandler.minion.name << "\n";
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create MonitorRequestHandler\n";

//     MonitorRequestHandler* monitorRequestHandler;
//     monitorRequestHandler = new MonitorRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }