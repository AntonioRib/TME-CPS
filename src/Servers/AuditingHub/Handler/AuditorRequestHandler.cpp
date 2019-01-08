#include "AuditorRequestHandler.h"

// const int MESSAGE_BYTES = 2048;

AuditorRequestHandler::AuditorRequestHandler() {
    std::cout << "AuditorRequestHandler created\n";
}

AuditorRequestHandler::AuditorRequestHandler(AuditingHub* auditingHub) : auditingHub{auditingHub} {
    std::cout << "AuditorRequestHandler created with the name " << auditingHub->getHostname() << "\n";
}

void AuditorRequestHandler::processAttestation(int clientSocket, std::string nonce, AuditingHub& auditingHub) {
    char buffer[SocketUtils::MESSAGE_BYTES];
    std::string configuration = Messages::QUOTE + " " + AttestationConstants::QUOTE;
    General::stringToCharArray(configuration, buffer, SocketUtils::MESSAGE_BYTES);
    // cout << "Buffer: " << buffer;
    SocketUtils::sendBuffer(clientSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugAuditingHub)
        cout << "Wrote: " << buffer << " to client\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(clientSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugAuditingHub)
        cout << "Recieved: " << buffer << "\n";

    string approved(buffer);
    vector<string> approvedSplit = General::splitString(approved);

    if (approvedSplit[0] == Messages::NOT_APPROVED) {
        cout << "Not approved!\n";
    } else if (approvedSplit[0] == Messages::OK_APPROVED) {
        cout << "Approved!\n";
        cout << "Configuration approved. Auditor signature for AuditingHub: " + approvedSplit[1] + ". For minions:" + approvedSplit[3];
        unsigned char* approvedConfiguration = (unsigned char*)approvedSplit[1].c_str();
        auditingHub.setApprovedConfiguration(approvedConfiguration);
        auditingHub.setApprovedSHA1Configuration(approvedSplit[2]);
    }
}

void AuditorRequestHandler::startAuditorRequestHandler(AuditorRequestHandler auditorRequestHandler) {
    std::cout << "AuditorRequestHandler running with Auditing Hub with the name " << auditorRequestHandler.auditingHub->getHostname() << "\n";

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::AHUB_AUDITOR_PORT);

    int serverSocket;
    serverSocket = SocketUtils::createServerSocket(serverAddress);

    int clientSocket;
    while (true) {
        clientSocket = SocketUtils::acceptClientSocket(serverSocket);
        cout << "Got connection from client\n";

        char buffer[SocketUtils::MESSAGE_BYTES];
        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::receiveBuffer(clientSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);

        if (DebugFlags::debugAuditingHub)
            cout << "Recieved: " << buffer << "\n";

        string command(buffer);
        vector<string> commandSplit = General::splitString(command);

        if (commandSplit[0] == Messages::ATTEST) {
            auditorRequestHandler.processAttestation(clientSocket, AttestationConstants::NONCE, *(auditorRequestHandler.auditingHub));
        }
    }
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create AuditorRequestHandler\n";

//     AuditorRequestHandler* auditorRequestHandler;
//     auditorRequestHandler = new AuditorRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }