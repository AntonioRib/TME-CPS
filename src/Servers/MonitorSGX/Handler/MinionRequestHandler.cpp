#include "MinionRequestHandler.h"

// const int MESSAGE_BYTES = 2048;

MinionRequestHandler::MinionRequestHandler() {
    std::cout << "MinionRequestHandler created\n";
}

MinionRequestHandler::MinionRequestHandler(Monitor* monitor) : monitor{monitor} {
    std::cout << "MinionRequestHandler created with the name " << monitor->getHostname() << "\n";
}

sgx_enclave_id_t minionRequestHandler_eid = 0;

void MinionRequestHandler::attestMinion(int minionSocket) { 

    if (initialize_enclave(&minionRequestHandler_eid, "MinionRequestHandlerEnclave.token", "enclave.signed.so") < 0) {
        std::cout << "Fail to initialize enclave." << std::endl;
        return ;
    }
    
    sgx_status_t status = trustedAttestMinion(minionRequestHandler_eid, minionSocket, SocketUtils::MESSAGE_BYTES);
    std::cout << status << std::endl;
    if (status != SGX_SUCCESS) {
        std::cout << "Failed" << std::endl;
    }
    std::cout << "Success" << std::endl;
    
}

void MinionRequestHandler::startMinionRequestHandler(MinionRequestHandler minionRequestHandler) {
    std::cout << "MinionRequestHandler running with Monitor with the name " << minionRequestHandler.monitor->getHostname() << "\n";

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::MONITOR_MINION_PORT);

    int serverSocket;
    serverSocket = SocketUtils::createServerSocket(serverAddress);

    int minionSocket;
    while (true) {
        cout << "Waiting for connections...\n";
        try{
            minionSocket = SocketUtils::acceptClientSocket(serverSocket);
            cout << "Got connection from Minion\n";
            minionRequestHandler.attestMinion(minionSocket);

            char buffer[SocketUtils::MESSAGE_BYTES];
            bzero(buffer, SocketUtils::MESSAGE_BYTES);
            SocketUtils::receiveBuffer(minionSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
            if (DebugFlags::debugMonitor)
                cout << "Recieved: " << buffer << "\n";
            // cout << buffer;
            string command(buffer);
            vector<string> commandSplit = General::splitString(command);

            bool requestResult = true;
            if (commandSplit[0] == Messages::REGISTER) {
                socklen_t len = sizeof(serverAddress);
                char ip[32];
                getpeername(minionSocket, (struct sockaddr*)&serverAddress, &len);
                // inet_ntop(AF_INET, &serverAddress.sin_addr, ip, sizeof(ip));
                // string minionAddress(ip);
                // string minionAddress(inet_ntoa(serverAddress.sin_addr));
                string minionAddress = commandSplit[1];
                if (DebugFlags::debugMonitor)
                    cout << "Registering: " << minionAddress << "\n";
                minionRequestHandler.monitor->addNewMinion(minionAddress);
            }

            if (requestResult) {
                bzero(buffer, SocketUtils::MESSAGE_BYTES);
                std::string result = Messages::OK_APPROVED;
                General::stringToCharArray(result, buffer, SocketUtils::MESSAGE_BYTES);
                SocketUtils::sendBuffer(minionSocket, buffer, strlen(buffer), 0);
                if (DebugFlags::debugMinion)
                    cout << "Success \n";
            } else {
                bzero(buffer, SocketUtils::MESSAGE_BYTES);
                std::string result = Messages::NOT_APPROVED;
                General::stringToCharArray(result, buffer, SocketUtils::MESSAGE_BYTES);
                SocketUtils::sendBuffer(minionSocket, buffer, strlen(buffer), 0);
                if (DebugFlags::debugMinion)
                    cout << "Failed \n";
            }
        } catch (const exception& e){
            close(minionSocket);
            cout << e.what() << '\n';
        }
    }
    // printf("Here is the message: %s\n", buffer);

    close(minionSocket);
    close(serverSocket);
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create MinionRequestHandler\n";

//     MinionRequestHandler* minionRequestHandler;
//     minionRequestHandler = new MinionRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }