#include "AuditorRequestHandler.h"

// const int MESSAGE_BYTES = 2048;

AuditorRequestHandler::AuditorRequestHandler() {
    std::cout << "AuditorRequestHandler created\n";
}

AuditorRequestHandler::AuditorRequestHandler(Monitor* monitor) : monitor{monitor}, monitorStore{monitor->getHostname()+".jks"}{
    std::cout << "AuditorRequestHandler created with the name " << monitor->getHostname() << "\n";
}

sgx_enclave_id_t auditorRequestHandler_eid = 0;

void AuditorRequestHandler::processAttestation(int clientSocket, std::string nonce, Monitor& monitor){
    if (initialize_enclave(&auditorRequestHandler_eid, "AuditorRequestHandler.token", "enclave.signed.so") < 0) {
        std::cout << "Fail to initialize enclave." << std::endl;
        return;
    }
    
    char result[SocketUtils::MESSAGE_BYTES];// = (char*)malloc(sizeof(char)*SocketUtils::MESSAGE_BYTES);
    sgx_status_t status = trustedProcessAttestation(auditorRequestHandler_eid, clientSocket, result, SocketUtils::MESSAGE_BYTES, (char *)AttestationConstants::NONCE.c_str(), SocketUtils::MESSAGE_BYTES);
    if (status != SGX_SUCCESS) {
        std::cout << "Enclave call Failed" << std::endl;
        return;
    }
    std::cout << "Enclave call Success" << std::endl;
    string approved(result);
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
    // free(result);
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
