#include "Minion.h"
#include "Handler/MinionAHubRequestHandler.h"
#include "Handler/MonitorRequestHandler.h"
#include <iterator>
using namespace std;

const int MESSAGE_BYTES = 2048;

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

Minion::Minion(string ipAddress) : ipAddress{ipAddress} {
    // std::cout << "Minion created with the name " << Minion::name << " - string constructor \n";
}

Minion::Minion(string monitorHost, string hostname, string ipAddress) : monitorHost{monitorHost}, hostname{hostname}, ipAddress{ipAddress} {
    // std::cout << "Minion created with the name " << Minion::name << " - string constructor \n";
    Minion::startMonitorHandler();
    Minion::startAuditingHubHandler();
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
map<string, Application *> Minion::getApplications(){
    return Minion::minionApplications;
}

void Minion::addApp(Application *app){
    Minion::minionApplications.insert(pair<string, Application*>(app->getAppID(), app));
}

void Minion::removeApp(string appID){
    Minion::minionApplications.erase(appID);
}

void Minion::processAttestation(int monitorSocket){
    char buffer[MESSAGE_BYTES];
    bzero(buffer, MESSAGE_BYTES);
    SocketUtils::receiveBuffer(monitorSocket, buffer, MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugMinion)
        cout << "Recieved: " << buffer << "\n";

    string request(buffer);
    vector<string> requestSplit = General::splitString(request);

    if (requestSplit[0] == Messages::ATTEST){
        std::string response = Messages::QUOTE + " " + AttestationConstants::QUOTE;
        General::stringToCharArray(response, buffer, MESSAGE_BYTES);
        SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugMinion)
            cout << "Wrote: " << buffer << " to Monitor\n";

        bzero(buffer, MESSAGE_BYTES);
        SocketUtils::receiveBuffer(monitorSocket, buffer, MESSAGE_BYTES - 1, 0);
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
    sockaddr_in monitorAddress;
    monitorAddress = SocketUtils::createServerAddress(Ports::MONITOR_MINION_PORT);

    int monitorSocket = socket(AF_INET, SOCK_STREAM, 0);
    SocketUtils::connectToServerSocket(monitorSocket, monitorAddress);
    // int monitorSocket;
    // monitorSocket = SocketUtils::connectToServerSocket(monitorAddress);

    processAttestation(monitorSocket);

    char buffer[MESSAGE_BYTES];
    string registerString = Messages::REGISTER;
    General::stringToCharArray(registerString, buffer, MESSAGE_BYTES);
    SocketUtils::sendBuffer(monitorSocket, buffer, MESSAGE_BYTES - 1, 0);

    if (DebugFlags::debugMinion)
        cout << "Wrote: " << buffer << " to Monitor\n";

    bzero(buffer, MESSAGE_BYTES);
    SocketUtils::receiveBuffer(monitorSocket, buffer, MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugMinion)
        cout << "Recieved from Monitor: " << buffer << "\n";

    string response(buffer);
    vector<string> splittedResposnse = General::splitString(response);
    if (splittedResposnse[0] == Messages::OK){
        MinionMonitorRequestHandler minionMonitorRequestHandler = MinionMonitorRequestHandler(this);
        std::thread monitorRequestHandlerThread(MinionMonitorRequestHandler::startMinionMonitorRequestHandler, minionMonitorRequestHandler);
        return true;
    } else if (splittedResposnse[0] == Messages::NOT_OK){
        return false;
    }
    return false;
}

bool Minion::startAuditingHubHandler(){
    MinionAHubRequestHandler minionAhubRequestHandler = MinionAHubRequestHandler(this);
    std::thread ahubRequestHandlerThread(MinionAHubRequestHandler::startMinionAHubRequestHandler, minionAhubRequestHandler);
    return true;
}


// int main(int argc, char *argv[]) {
//     if (argc != 7) {
//         cout << "Usage: Minion -m monitorHostName -h hostname -i ipAddress\n";
//         return 0;
//     }

//     string monitorHostname(argv[MONITOR_HOST_FLAG_INDEX + 1]);
//     string hostname(argv[HOSTNAME_FLAG_INDEX + 1]);  //IPADDRESS_FLAG_INDEX
//     string ipAddress(argv[IPADDRESS_FLAG_INDEX + 1]);

//     std::cout << "Will try to create Minion\n";

//     //  std::string s = "Minion I";
//      Minion *minion = new Minion(monitorHostname, hostname, ipAddress);

//     //  MinionAHubRequestHandler minionAHubRequestHandler = MinionAHubRequestHandler(minion);
//     //  std::thread minionAhubRequestHandlerThread(MinionAHubRequestHandler::startMinionAHubRequestHandler, minionAHubRequestHandler);

//     //  MinionMonitorRequestHandler minionMonitorRequestHandler = MinionMonitorRequestHandler(minion);
//     //  std::thread monitorRequestHandlerThread(MinionMonitorRequestHandler::startMonitorRequestHandler, minionMonitorRequestHandler);

//     //  minionAhubRequestHandlerThread.join();
//     //  monitorRequestHandlerThread.join();

//      std::cout << "Bubye\n";

//      return 0;
//  }