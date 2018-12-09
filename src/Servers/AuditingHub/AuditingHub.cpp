#include "AuditingHub.h"

AuditingHub::AuditingHub()
{
    std::cout << "AuditingHub created\n";
}

int main(int argc, char *argv[])
{
    std::cout << "Will try to create AuditingHub\n";

    AuditingHub *auditingHub;
    auditingHub = new AuditingHub();

    std::cout << "Bubye\n";

    return 0;
}