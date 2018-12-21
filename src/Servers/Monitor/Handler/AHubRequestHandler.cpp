#include "AHubRequestHandler.h"

AHubRequestHandler::AHubRequestHandler() {
    std::cout << "AHubRequestHandler created\n";
}

AHubRequestHandler::AHubRequestHandler(Monitor m): monitor(m) {
    std::cout << "AHubRequestHandler created with the name " << monitor.name << "\n";
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create AHubRequestHandler\n";

//     AHubRequestHandler* ahubRequestHandler;
//     ahubRequestHandler = new AHubRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }