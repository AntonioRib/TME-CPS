#include "MinionRequestHandler.h"

MinionRequestHandler::MinionRequestHandler() {
    std::cout << "MinionRequestHandler created\n";
}

int main(int argc, char* argv[]) {
    std::cout << "Will try to create MinionRequestHandler\n";

    MinionRequestHandler* minionRequestHandler;
    minionRequestHandler = new MinionRequestHandler();

    std::cout << "Bubye\n";

    return 0;
}