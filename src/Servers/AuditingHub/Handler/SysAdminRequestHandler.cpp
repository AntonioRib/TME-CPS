#include "SysAdminRequestHandler.h"

SysAdminRequestHandler::SysAdminRequestHandler() {
    std::cout << "SysAdminRequestHandler created\n";
}

SysAdminRequestHandler::SysAdminRequestHandler(AuditingHub auditingHub) : auditingHub{auditingHub} {
    std::cout << "SysAdminRequestHandler created with the name " << auditingHub.name << "\n";
}

void SysAdminRequestHandler::startSysAdminRequestHandler(SysAdminRequestHandler sysAdminRequestHandler) {
    std::cout << "SysAdminRequestHandler running with Auditing Hub with the name " << sysAdminRequestHandler.auditingHub.name << "\n";
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create SysAdminRequestHandler\n";

//     SysAdminRequestHandler* sysAdminRequestHandler;
//     sysAdminRequestHandler = new SysAdminRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }