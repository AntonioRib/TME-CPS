#include "AuditingHub.h"
#include "Handler/AuditorRequestHandler.h"
#include "Handler/SysAdminRequestHandler.h"

AuditingHub::AuditingHub()
{
    std::cout << "AuditingHub created\n";
}

AuditingHub::AuditingHub(const AuditingHub &m) : hostname{m.hostname} {
    // std::cout << "AuditingHub created with the name " << AuditingHub::name << " - copy constructor \n";
}

AuditingHub::AuditingHub(std::string hostname) : hostname{hostname} {
    // std::cout << "AuditingHub created with the name " << AuditingHub::name << " - string constructor \n";
}

//Getters
string AuditingHub::getUsername(){
    return AuditingHub::username;
}

string AuditingHub::getHubKey() {
    return AuditingHub::hubKey;
}

string AuditingHub::getMonitorHost(){
    return AuditingHub::monitorHost;
}

string AuditingHub::getHostname(){
    return AuditingHub::hostname;
}

unsigned char* AuditingHub::getApprovedConfiguration() {
    return AuditingHub::approvedConfiguration;
}

string AuditingHub::getApprovedSHA1(){
    return AuditingHub::approvedSHA1;
}

//Setters
void AuditingHub::setApprovedConfiguration(unsigned char* approvedConfiguration){
    AuditingHub::approvedConfiguration = approvedConfiguration;
}

void AuditingHub::setApprovedSH1Configuration(string approvedSHA1Configuration){
    AuditingHub::approvedSHA1 = approvedSHA1Configuration;
}

//Functions
bool AuditingHub::checkPermissionAndUnqueue(string remoteHost){
    //TODO
}

void AuditingHub::removeSession(string remoteHost) {
    //TODO
}


int main(int argc, char *argv[]) {
    std::cout << "Will try to create AuditingHub\n";

    std::string s = "AuditingHub I";
    AuditingHub* auditingHub = new AuditingHub(s);

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