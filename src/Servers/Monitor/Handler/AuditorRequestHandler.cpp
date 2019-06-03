#include "AuditorRequestHandler.h"
#include <chrono>

// const int MESSAGE_BYTES = 2048;

AuditorRequestHandler::AuditorRequestHandler() {
    std::cout << "AuditorRequestHandler created\n";
}

AuditorRequestHandler::AuditorRequestHandler(Monitor* monitor) : monitor{monitor}, monitorStore{monitor->getHostname()+".jks"}{
    std::cout << "AuditorRequestHandler created with the name " << monitor->getHostname() << "\n";
}

void AuditorRequestHandler::processAttestation(int clientSocket, std::string nonce, Monitor& monitor){
    char buffer[SocketUtils::MESSAGE_BYTES];
    // TPM2B_ATTEST *quoted = NULL;
	// TPMT_SIGNATURE *signature = NULL;
    // TPM::tpm_quote(quoted, signature);
    #pragma region Debug
    // cout << "quoted: " << quoted << "\n";
    // cout << "signature: " << quoted << "\n";
#pragma endregion
    std::string configuration = Messages::QUOTE + " " + AttestationConstants::QUOTE;
    General::stringToCharArray(configuration, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(clientSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugMonitor)
        cout << "Wrote: " << buffer << " to client\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(clientSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugMonitor)
        cout << "Recieved: " << buffer << "\n";

    string approved(buffer);
    vector<string> approvedSplit = General::splitString(approved);

    if (approvedSplit[0] == Messages::NOT_APPROVED) {
        cout << "Not approved!\n";
    } else if (approvedSplit[0] == Messages::OK_APPROVED) {
        cout << "Approved!\n";
        cout << "Configuration approved. Auditor signature for monitor: " + approvedSplit[1] + ". For minions:" + approvedSplit[3];
        unsigned char * approvedConfiguration = (unsigned char *) approvedSplit[1].c_str();
        monitor.setApprovedConfiguration(approvedConfiguration, approvedSplit[2]);
        monitor.setApprovedConfigurationForMinions(approvedSplit[3]);
        cout << "Approved Configuration " << +monitor.getApprovedConfiguration();
    }
}

void AuditorRequestHandler::startAuditorRequestHandler(AuditorRequestHandler auditorRequestHandler) {
    std::cout << "Trying to create AuditorRequestHandler with Monitor with the name " << auditorRequestHandler.monitor->getHostname() << "\n";

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::MONITOR_AUDITOR_PORT);

    int serverSocket;
    serverSocket = SocketUtils::createServerSocket(serverAddress);

    int clientSocket;
    while(true){
        cout << "Waiting for connections...\n";
        try {
            clientSocket = SocketUtils::acceptClientSocket(serverSocket);
            cout << "Got connection from client\n";

            char buffer[SocketUtils::MESSAGE_BYTES];
            bzero(buffer, SocketUtils::MESSAGE_BYTES);
            SocketUtils::receiveBuffer(clientSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
            if (DebugFlags::debugMonitor)
                cout << "Recieved: " << buffer << "\n";
            // cout << buffer;
            string command(buffer);
            vector<string> commandSplit = General::splitString(command);
            
            if (commandSplit[0] == Messages::ATTEST){
                auditorRequestHandler.processAttestation(clientSocket, AttestationConstants::QUOTE, *(auditorRequestHandler.monitor));
            }
        } catch (const exception& e){
            close(clientSocket);
            cout << e.what() << '\n';
        }
    }
    // printf("Here is the message: %s\n", buffer);

    close(clientSocket);
    close(serverSocket);
}
