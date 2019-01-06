#include "AHubRequestHandler.h"
// #include "../../../Utilities/httpLib/httplib.h"

AHubRequestHandler::AHubRequestHandler() {
    std::cout << "AHubRequestHandler created\n";
}

bool sendApp(Minion* minion, std::string appDir){
    //TODO
}

bool deployAppOnMinion(std::string appId, Minion* host) {
    //TODO
}

bool spawnReplacementInstances(Minion* untrustedMinion){
    //TODO
}

void attestMinion(std::string untrustedMinion){
    //TODO
}

AHubRequestHandler::AHubRequestHandler(Monitor* monitor) : monitor{monitor} {
    std::cout << "AHubRequestHandler created with the name " << monitor->getHostname() << "\n";
}

void AHubRequestHandler::startAHubRequestHandler(AHubRequestHandler aHubRequestHandler) {
    std::cout << "AHubRequestHandler running with Monitor with the name " << aHubRequestHandler.monitor->getHostname() << "\n";
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create AHubRequestHandler\n";

//     AHubRequestHandler* ahubRequestHandler;
//     ahubRequestHandler = new AHubRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }