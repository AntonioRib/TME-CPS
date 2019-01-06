#include "AuditingHub.h"
#include <algorithm>
#include <chrono>
#include <iterator>
#include <sstream>
#include "Handler/AuditorRequestHandler.h"
#include "Handler/SysAdminRequestHandler.h"

const int HUB_USERNAME_FLAG_INDEX = 1;
const int HUB_KEY_FLAG_INDEX = 3;
const int MONITOR_FLAG_INDEX = 5;
const int HOSTNAME_FLAG_INDEX = 7;

const unsigned char *NULL_VALUE = (unsigned char *)"\0";

AuditingHub::AuditingHub()
{
    std::cout << "AuditingHub created\n";
}

AuditingHub::AuditingHub(const AuditingHub &m){
    AuditingHub::approvedConfiguration = (unsigned char *)NULL_VALUE;
    // std::cout << "AuditingHub created with the name " << AuditingHub::name << " - copy constructor \n";
}

AuditingHub::AuditingHub(string hubUserName, string hubKey, string monitorHost, string hotsName)
    : hubUserName{hubUserName}, hubKey{hubKey}, monitorHost{monitorHost}, hostname{hostname} {
    AuditingHub::approvedConfiguration = (unsigned char *)NULL_VALUE;
    // std::cout << "AuditingHub created with the name " << AuditingHub::name << " - string constructor \n";
}

//Getters
string AuditingHub::getHubUserName(){
    return AuditingHub::hubUserName;
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

map<long, thread*> AuditingHub::getTemporaryThreads() {
    return AuditingHub::temporaryThreads;
}

map<string, thread*> AuditingHub::getRemoteHostThreadMap() {
    return AuditingHub::remoteHostThreadMap;
}

//Setters
void AuditingHub::setApprovedConfiguration(unsigned char* approvedConfiguration){
    AuditingHub::approvedConfiguration = approvedConfiguration;
}

void AuditingHub::setApprovedSHA1Configuration(string approvedSHA1Configuration){
    AuditingHub::approvedSHA1 = approvedSHA1Configuration;
}

//Functions
bool AuditingHub::checkPermissionAndUnqueue(string remoteHost){
    thread* sessionThread = AuditingHub::remoteHostThreadMap.find(remoteHost)->second;
    if (sessionThread != NULL){
        return false;
    } else {
        std::ostringstream ss;
        ss << this_thread::get_id();
        std::string idstr = ss.str();

        thread* th = temporaryThreads.find(atol(idstr.c_str()))->second;
        AuditingHub::remoteHostThreadMap.insert(pair<string, thread*>(remoteHost, th));
        temporaryThreads.erase(atol(idstr.c_str()));
        return true;
    }
}

void AuditingHub::removeSession(string remoteHost) {
    AuditingHub::remoteHostThreadMap.erase(remoteHost);
}


int main(int argc, char *argv[]) {
    if (argc != 9)
    {
        cout << "Usage: AudtingHub -u userName -k key -m monitor -h hostname\n";
        return 0;
    }

    string hubUserName(argv[HUB_USERNAME_FLAG_INDEX + 1]);
    string hubKey(argv[HUB_KEY_FLAG_INDEX + 1]);
    string monitorHost(argv[MONITOR_FLAG_INDEX + 1]);
    string hostname(argv[HOSTNAME_FLAG_INDEX + 1]);

    std::cout << "Will try to create AuditingHub\n";

    std::string s = "AuditingHub I";
    AuditingHub *auditingHub = new AuditingHub(hubUserName, hubKey, monitorHost, hostname);

    AuditorRequestHandler auditorRequestHandler = AuditorRequestHandler(auditingHub);
    std::thread auditorRequestHandlerThread(AuditorRequestHandler::startAuditorRequestHandler, auditorRequestHandler);

    while (auditingHub->getApprovedConfiguration() == (unsigned char *)NULL_VALUE)
    {
        std::cout << "Im in the loop \n";
        printf("This prints this: %s", auditingHub->getApprovedConfiguration());
        std::cout << "Waiting for approved config. Current Config -> " << +auditingHub->getApprovedConfiguration() << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::AHUB_SYSADMIN_PORT);

    int serverSocket;
    serverSocket = SocketUtils::createServerSocket(serverAddress);

    thread sessionThread;
    int clientSocket;
    while (true){
        clientSocket = SocketUtils::acceptClientSocket(serverSocket);
        cout << "Management request\n";

        SysAdminRequestHandler sysAdminRequestHandler = SysAdminRequestHandler(auditingHub);
        sessionThread = thread(SysAdminRequestHandler::startSysAdminRequestHandler, sysAdminRequestHandler);

        std::ostringstream ss;
        ss << sessionThread.get_id();
        std::string idstr = ss.str();

        auditingHub->getTemporaryThreads().insert(pair<long, thread *>(atol(idstr.c_str()), &sessionThread));
    }

    auditorRequestHandlerThread.join();
    // sysAdminRequestHandlerThread.join();

    std::cout << "Bubye\n";

    return 0;
}