#include "AuditorRequestHandler.h"

// const int MESSAGE_BYTES = 2048;

AuditorRequestHandler::AuditorRequestHandler() {
    std::cout << "AuditorRequestHandler created\n";
}

AuditorRequestHandler::AuditorRequestHandler(AuditingHub* auditingHub) : auditingHub{auditingHub} {
    std::cout << "AuditorRequestHandler created with the name " << auditingHub->getHostname() << "\n";
}

sgx_enclave_id_t auditorRequestHandler_eid = 0;

void AuditorRequestHandler::processAttestation(int clientSocket, std::string nonce, AuditingHub& auditingHub) {
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
        cout << "Configuration approved. Auditor signature for monitor: " + approvedSplit[1] + ". For minions:" + approvedSplit[3] + "\n";
        unsigned char* approvedConfiguration = (unsigned char*)approvedSplit[1].c_str();
        auditingHub.setApprovedConfiguration(approvedConfiguration);
        auditingHub.setApprovedSHA1Configuration(approvedSplit[2]);
    }
    // free(result);
}

void AuditorRequestHandler::startAuditorRequestHandler(AuditorRequestHandler auditorRequestHandler) {
    std::cout << "AuditorRequestHandler running with Auditing Hub with the name " << auditorRequestHandler.auditingHub->getHostname() << "\n";

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::AHUB_AUDITOR_PORT);

    int serverSocket;
    serverSocket = SocketUtils::createServerSocket(serverAddress);

    int clientSocket;
    while (true) {
        cout << "Waiting for connections...\n";
        try {
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
        } catch (const exception& e){
            close(clientSocket);
            cout << e.what() << '\n';
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