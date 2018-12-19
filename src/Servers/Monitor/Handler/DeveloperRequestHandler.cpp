#include "DeveloperRequestHandler.h"

DeveloperRequestHandler::DeveloperRequestHandler() {
    std::cout << "DeveloperRequestHandler created\n";
}

DeveloperRequestHandler::DeveloperRequestHandler(Monitor* monitor) {
    std::cout << "DeveloperRequestHandler created\n";
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create DeveloperRequestHandler\n";

//     DeveloperRequestHandler* developerRequestHandler;
//     developerRequestHandler = new DeveloperRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }