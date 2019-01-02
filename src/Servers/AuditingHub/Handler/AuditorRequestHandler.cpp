#include "AuditorRequestHandler.h"

AuditorRequestHandler::AuditorRequestHandler() {
    std::cout << "AuditorRequestHandler created\n";
}

AuditorRequestHandler::AuditorRequestHandler(AuditingHub* auditingHub) : auditingHub{auditingHub} {
    std::cout << "AuditorRequestHandler created with the name " << auditingHub->getHostname() << "\n";
}

void AuditorRequestHandler::startAuditorRequestHandler(AuditorRequestHandler auditorRequestHandler) {
    std::cout << "AuditorRequestHandler running with Auditing Hub with the name " << auditorRequestHandler.auditingHub->getHostname() << "\n";
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create AuditorRequestHandler\n";

//     AuditorRequestHandler* auditorRequestHandler;
//     auditorRequestHandler = new AuditorRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }