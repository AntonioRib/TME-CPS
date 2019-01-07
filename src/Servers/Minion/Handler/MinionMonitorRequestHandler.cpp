#include "MinionMonitorRequestHandler.h"

MinionMonitorRequestHandler::MinionMonitorRequestHandler() {
    std::cout << "MonitorRequestHandler created\n";
}

MinionMonitorRequestHandler::MinionMonitorRequestHandler(Minion* minion) : minion{minion} {
    std::cout << "MonitorRequestHandler created with the name " << minion->getIpAddress() << "\n";
}

void MinionMonitorRequestHandler::MinionMonitorRequestHandler(MinionMonitorRequestHandler minionMonitorRequestHandler) {
    std::cout << "MonitorRequestHandler running with Minion with the name " << minionMonitorRequestHandler.minion->getIpAddress() << "\n";
}

bool MinionMonitorRequestHandler::deployApp(string appId) {
    //TODO
}

bool MinionMonitorRequestHandler::deleteApp(string appId) {
    //TODO
}

void MinionMonitorRequestHandler::processAttestation(string nonce) {  //TODO
    //TODO
}  

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create MonitorRequestHandler\n";

//     MonitorRequestHandler* monitorRequestHandler;
//     monitorRequestHandler = new MonitorRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }