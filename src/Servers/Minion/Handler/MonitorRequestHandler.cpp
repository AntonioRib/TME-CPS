#include "MonitorRequestHandler.h"

MonitorRequestHandler::MonitorRequestHandler() {
    std::cout << "MonitorRequestHandler created\n";
}

int main(int argc, char* argv[]) {
    std::cout << "Will try to create MonitorRequestHandler\n";

    MonitorRequestHandler* monitorRequestHandler;
    monitorRequestHandler = new MonitorRequestHandler();

    std::cout << "Bubye\n";

    return 0;
}