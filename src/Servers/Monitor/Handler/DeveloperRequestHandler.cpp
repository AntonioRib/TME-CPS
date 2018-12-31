#include "DeveloperRequestHandler.h"

DeveloperRequestHandler::DeveloperRequestHandler() {
    std::cout << "DeveloperRequestHandler created\n";
}

DeveloperRequestHandler::DeveloperRequestHandler(Monitor* monitor) : monitor{monitor} {
    std::cout << "DeveloperRequestHandler created with the name " << monitor->hostname << "\n";
}

void DeveloperRequestHandler::startDeveloperRequestHandler(DeveloperRequestHandler developerRequestHandler) {
    std::cout << "DeveloperRequestHandler running with Monitor with the name " << developerRequestHandler.monitor->hostname << "\n";
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create DeveloperRequestHandler\n";

//     DeveloperRequestHandler* developerRequestHandler;
//     developerRequestHandler = new DeveloperRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }