#include "AuditingHub.h"
#include "Handler/AuditorRequestHandler.h"
#include "Handler/SysAdminRequestHandler.h"

AuditingHub::AuditingHub()
{
    std::cout << "AuditingHub created\n";
}

AuditingHub::AuditingHub(const AuditingHub &m) : name{m.name} {
    // std::cout << "AuditingHub created with the name " << AuditingHub::name << " - copy constructor \n";
}

AuditingHub::AuditingHub(std::string n) : name{n} {
    // std::cout << "AuditingHub created with the name " << AuditingHub::name << " - string constructor \n";
}

int main(int argc, char *argv[]) {
    std::cout << "Will try to create AuditingHub\n";

    std::string s = "AuditingHub I";
    AuditingHub auditingHub = AuditingHub(s);

    AuditorRequestHandler auditorRequestHandler = AuditorRequestHandler(auditingHub);
    std::thread auditorRequestHandlerThread(AuditorRequestHandler::startAuditorRequestHandler, auditorRequestHandler);

    SysAdminRequestHandler sysAdminRequestHandler = SysAdminRequestHandler(auditingHub);
    std::thread sysAdminRequestHandlerThread(SysAdminRequestHandler::startSysAdminRequestHandler, sysAdminRequestHandler);

    auditorRequestHandlerThread.join();
    sysAdminRequestHandlerThread.join();

    std::cout << "Bubye\n";

    return 0;
}

// int main(int argc, char *argv[])
// {
//     std::cout << "Will try to create AuditingHub\n";

//     AuditingHub *auditingHub;
//     auditingHub = new AuditingHub();

//     std::cout << "Bubye\n";

//     return 0;
// }