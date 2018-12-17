#include "AuditorsRequestHandler.h"

AuditorRequestHandler::AuditorRequestHandler() {
    std::cout << "AuditorRequestHandler created\n";
}

int main(int argc, char* argv[]) {
    std::cout << "Will try to create AuditorRequestHandler\n";

    AuditorRequestHandler* auditorRequestHandler;
    auditorRequestHandler = new AuditorRequestHandler();

    std::cout << "Bubye\n";

    return 0;
}