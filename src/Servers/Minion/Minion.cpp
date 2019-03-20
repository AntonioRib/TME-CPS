#include "Minion.h"
#include "Handler/MinionAHubRequestHandler.h"
#include "Handler/MinionMonitorRequestHandler.h"
#include <iterator>
using namespace std;

// const int MESSAGE_BYTES = 2048;

const int MONITOR_HOST_FLAG_INDEX = 1;
const int HOSTNAME_FLAG_INDEX = 3;
const int IPADDRESS_FLAG_INDEX = 5;

Minion::Minion()
{
    std::cout << "Minion created\n";
}

Minion::Minion(const Minion &m) : monitorHost{m.monitorHost}, hostname{m.hostname}, ipAddress{m.ipAddress} {
    // std::cout << "Minion created with the name " << Minion::name << " - copy constructor \n";
    Minion::startMonitorHandler();
    Minion::startAuditingHubHandler();
}

Minion::Minion(string monitorHost, string hostname, string ipAddress) : monitorHost{monitorHost}, hostname{hostname}, ipAddress{ipAddress} {
    std::cout << "Minion created with the name " << Minion::monitorHost << " - string constructor \n";
    bool handlersStartResult = true;
    handlersStartResult &= Minion::startMonitorHandler();
    handlersStartResult &= Minion::startAuditingHubHandler();

    if (handlersStartResult){
        while (true) {
            this_thread::yield();
        }
    }
}

string Minion::getMonitorHost(){
    return Minion::monitorHost;
}
string Minion::getHostname(){
    return Minion::hostname;
}

string Minion::getIpAddress(){
    return Minion::ipAddress;
}

void Minion::processAttestation(int monitorSocket){
    char buffer[SocketUtils::MESSAGE_BYTES];
    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugMinion)
        cout << "Recieved: " << buffer << "\n";

    string request(buffer);
    vector<string> requestSplit = General::splitString(request);

    if (requestSplit[0] == Messages::ATTEST){
        std::string response = Messages::QUOTE + " " + AttestationConstants::QUOTE;
        General::stringToCharArray(response, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugMinion)
            cout << "Wrote: " << buffer << " to Monitor\n";

        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::receiveBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
        if (DebugFlags::debugMinion)
            cout << "Recieved: " << buffer << "\n";

        string result(buffer);
        vector<string> resultSplit = General::splitString(result);
        if(resultSplit[0] == Messages::NOT_APPROVED)
            cout << "Rejected Configuration: " << buffer << "\n";
        else if (resultSplit[0] == Messages::OK_APPROVED) {
            cout << "Approved!\n";
        }
    }
}

bool Minion::startMonitorHandler(){
    struct hostent* serverHost;
    serverHost = SocketUtils::getHostByName(Minion::monitorHost);
    cout << "Server host address: " << (char *)serverHost->h_addr << "\n";

    sockaddr_in monitorAddress;
    monitorAddress = SocketUtils::createServerAddress(Ports::MONITOR_MINION_PORT+20);
    bcopy((char *)serverHost->h_addr, (char *)&monitorAddress.sin_addr.s_addr, serverHost->h_length);
    
    int monitorSocket = socket(AF_INET, SOCK_STREAM, 0);
    SocketUtils::connectToServerSocket(monitorSocket, monitorAddress);
    if (DebugFlags::debugAuditor)
        cout << "Connected to the Monitor\n";

    processAttestation(monitorSocket);

    char buffer[SocketUtils::MESSAGE_BYTES];
    string registerString = Messages::REGISTER;
    General::stringToCharArray(registerString, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);

    if (DebugFlags::debugMinion)
        cout << "Wrote: " << buffer << " to Monitor\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugMinion)
        cout << "Recieved from Monitor: " << buffer << "\n";

    string response(buffer);
    vector<string> splittedResposnse = General::splitString(response);
    if (splittedResposnse[0] == Messages::OK_APPROVED){
        MinionMonitorRequestHandler* minionMonitorRequestHandler = new MinionMonitorRequestHandler(this);
        std::thread monitorRequestHandlerThread(MinionMonitorRequestHandler::startMinionMonitorRequestHandler, minionMonitorRequestHandler);
        monitorRequestHandlerThread.detach();
        return true;
    } else if (splittedResposnse[0] == Messages::NOT_APPROVED){
        return false;
    }
    return false;
}

bool Minion::startAuditingHubHandler(){
    MinionAHubRequestHandler* minionAhubRequestHandler = new MinionAHubRequestHandler(this);
    std::thread ahubRequestHandlerThread(MinionAHubRequestHandler::startMinionAHubRequestHandler, minionAhubRequestHandler);
    ahubRequestHandlerThread.detach();
    return true;
}


int main(int argc, char *argv[]) {
    if (argc != 7) {
        cout << "Usage: Minion -m monitorHostName -h hostname -i ipAddress\n";
        return 0;
    }
    cout << argv[0] << "\n";
    string monitorHostname(argv[MONITOR_HOST_FLAG_INDEX + 1]);
    string hostname(argv[HOSTNAME_FLAG_INDEX + 1]);  //IPADDRESS_FLAG_INDEX
    string ipAddress(argv[IPADDRESS_FLAG_INDEX + 1]);

    std::cout << "Will try to create Minion\n";

    //  std::string s = "Minion I";
     Minion *minion = new Minion(monitorHostname, hostname, ipAddress);

     std::cout << "Bubye\n";

     return 0;
 }