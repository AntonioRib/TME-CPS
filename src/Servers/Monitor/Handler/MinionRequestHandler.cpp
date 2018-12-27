#include "MinionRequestHandler.h"

MinionRequestHandler::MinionRequestHandler() {
    std::cout << "MinionRequestHandler created\n";
}

MinionRequestHandler::MinionRequestHandler(Monitor monitor) : monitor{monitor} {
    std::cout << "MinionRequestHandler created with the name " << monitor.username << "\n";
}

void MinionRequestHandler::startMinionRequestHandler(MinionRequestHandler minionRequestHandler) {
    std::cout << "MinionRequestHandler running with Monitor with the name " << minionRequestHandler.monitor.username << "\n";
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create MinionRequestHandler\n";

//     MinionRequestHandler* minionRequestHandler;
//     minionRequestHandler = new MinionRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }