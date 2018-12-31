#include "AHubRequestHandler.h"
#include "../../../Utilities/httpLib/httplib.h"

AHubRequestHandler::AHubRequestHandler() {
    std::cout << "AHubRequestHandler created\n";
}

AHubRequestHandler::AHubRequestHandler(Monitor* monitor) : monitor{monitor} {
    std::cout << "AHubRequestHandler created with the name " << monitor->hostname << "\n";
}

void AHubRequestHandler::startAHubRequestHandler(AHubRequestHandler aHubRequestHandler) {
    std::cout << "AHubRequestHandler running with Monitor with the name " << aHubRequestHandler.monitor->hostname << "\n";
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create AHubRequestHandler\n";

//     AHubRequestHandler* ahubRequestHandler;
//     ahubRequestHandler = new AHubRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }