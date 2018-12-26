#include "AHubRequestHandler.h"

AHubRequestHandler::AHubRequestHandler() {
    std::cout << "AHubRequestHandler created\n";
}

AHubRequestHandler::AHubRequestHandler(Minion minion) : minion{minion} {
    std::cout << "AHubRequestHandler created with the name " << minion.name << "\n";
}

void AHubRequestHandler::startAHubRequestHandler(AHubRequestHandler aHubRequestHandler) {
    std::cout << "AHubRequestHandler running with Minion with the name " << aHubRequestHandler.minion.name << "\n";
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create AHubRequestHandler\n";

//     AHubRequestHandler* ahubRequestHandler;
//     ahubRequestHandler = new AHubRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }