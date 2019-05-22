#include "MinionAuditorRequestHandler.h"
using namespace std;

const int ERROR_CODE = 255;

MinionAuditorRequestHandler::MinionAuditorRequestHandler() {
    std::cout << "MinionAuditorRequestHandler created\n";
}

MinionAuditorRequestHandler::MinionAuditorRequestHandler(Minion* minion) : minion{minion} {
    std::cout << "MinionAuditorRequestHandler created with the name " << minion->getIpAddress() << "\n";
}

bool MinionAuditorRequestHandler::processAttestation(int monitorSocket, string nonce) {
    char buffer[SocketUtils::MESSAGE_BYTES];
    std::string configuration = Messages::QUOTE + " " + AttestationConstants::QUOTE;
    General::stringToCharArray(configuration, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugMinion)
        cout << "Wrote: " << buffer << " to client\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugMinion)
        cout << "Recieved: " << buffer << "\n";

    string approved(buffer);
    vector<string> approvedSplit = General::splitString(approved);

    if (approvedSplit[0] == Messages::NOT_APPROVED) {
        cout << "Not approved!\n";
        return false;
    } else if (approvedSplit[0] == Messages::OK_APPROVED) {
        cout << "Approved!\n";
        return true;
        // cout << "Configuration approved. Auditor signature for monitor: " + approvedSplit[1] + ". For minions:" + approvedSplit[3];
    }
    return false;
}

void MinionAuditorRequestHandler::startMinionAuditorRequestHandler(MinionAuditorRequestHandler* minionAuditorRequestHandler) {
    std::cout << "MinionAuditorRequestHandler running with Minion with the name " << minionAuditorRequestHandler->minion->getIpAddress() << "\n";

    sockaddr_in auditorAddress;
    auditorAddress = SocketUtils::createServerAddress(Ports::MINION_AUDITOR_PORT);

    int auditorSocket;
    auditorSocket = SocketUtils::createServerSocket(auditorAddress);

    // if (DebugFlags::debugMinion)
    //     cout << "MinionAuditorRequestHandler Created socket to Monitor\n";

    int monitorSocket;
    while (true) {
        cout << "Waiting for connections...\n";
        try {
         
            if (DebugFlags::debugMinion)
                cout << "MinionMonitorRequestHandler Waiting for connection\n";
            monitorSocket = SocketUtils::acceptClientSocket(auditorSocket);
            cout << "Got connection from Auditor\n";

            char buffer[SocketUtils::MESSAGE_BYTES];
            bzero(buffer, SocketUtils::MESSAGE_BYTES);
            SocketUtils::receiveBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
            if (DebugFlags::debugMinion)
                cout << "Recieved: " << buffer << "\n";
            // cout << buffer;
            string command(buffer);
            vector<string> commandSplit = General::splitString(command);

            bool requestResult = false;
            if (commandSplit[0] == Messages::ATTEST) {
                requestResult = minionAuditorRequestHandler->processAttestation(monitorSocket, commandSplit[1]);
            }

            if (requestResult) {
                bzero(buffer, SocketUtils::MESSAGE_BYTES);
                std::string result = Messages::OK;
                General::stringToCharArray(result, buffer, SocketUtils::MESSAGE_BYTES);
                SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
                if (DebugFlags::debugMinion)
                    cout << "Success \n";
            } else {
                bzero(buffer, SocketUtils::MESSAGE_BYTES);
                std::string result = Messages::NOT_OK;
                General::stringToCharArray(result, buffer, SocketUtils::MESSAGE_BYTES);
                SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
                if (DebugFlags::debugMinion)
                    cout << "Failed \n";
            }
        } catch (const exception& e){
            close(monitorSocket);
            cout << e.what() << '\n';
        }
    }
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create MonitorRequestHandler\n";

//     MonitorRequestHandler* monitorRequestHandler;
//     monitorRequestHandler = new MonitorRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }