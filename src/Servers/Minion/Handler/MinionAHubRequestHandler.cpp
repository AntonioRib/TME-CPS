#include "MinionAHubRequestHandler.h"

MinionAHubRequestHandler::MinionAHubRequestHandler() {
    std::cout << "AHubRequestHandler created\n";
}

MinionAHubRequestHandler::MinionAHubRequestHandler(Minion* minion) : minion{minion} {
    std::cout << "AHubRequestHandler created with the name " << minion->getIpAddress() << "\n";
}

void MinionAHubRequestHandler::startMinionAHubRequestHandler(MinionAHubRequestHandler minionAHubRequestHandler) {
    std::cout << "AHubRequestHandler running with Minion with the name " << minionAHubRequestHandler.minion->getIpAddress() << "\n";
}

bool MinionAHubRequestHandler::purgeMinion() {
    //TODO
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create AHubRequestHandler\n";

//     AHubRequestHandler* ahubRequestHandler;
//     ahubRequestHandler = new AHubRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }