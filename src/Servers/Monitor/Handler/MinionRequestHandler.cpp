#include "MinionRequestHandler.h"

// const int MESSAGE_BYTES = 2048;

MinionRequestHandler::MinionRequestHandler() {
    std::cout << "MinionRequestHandler created\n";
}

MinionRequestHandler::MinionRequestHandler(Monitor* monitor) : monitor{monitor} {
    std::cout << "MinionRequestHandler created with the name " << monitor->getHostname() << "\n";
}

void MinionRequestHandler::attestMinion(int minionSocket) {
    char buffer[SocketUtils::MESSAGE_BYTES];
    std::string request = Messages::ATTEST + " " + AttestationConstants::NONCE;
    General::stringToCharArray(request, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(minionSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugMonitor)
        cout << "Wrote: " << buffer << " to Minion\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(minionSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugMonitor)
        cout << "Recieved: " << buffer << "\n";

    string response(buffer);
    vector<string> responseSplit = General::splitString(response);
    if (responseSplit[0] == Messages::QUOTE && responseSplit[1] == AttestationConstants::QUOTE) {
        std::string confirmation = Messages::OK;
        General::stringToCharArray(confirmation, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(minionSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugMonitor)
            cout << "Wrote: " << buffer << " to Minion\n";
    } else {
        std::string confirmation = Messages::NOT_OK;
        General::stringToCharArray(confirmation, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(minionSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugMonitor)
            cout << "Wrote: " << buffer << " to Minion\n";
        cout << "Not approved!\n";
    }
}

void MinionRequestHandler::startMinionRequestHandler(MinionRequestHandler minionRequestHandler) {
    std::cout << "MinionRequestHandler running with Monitor with the name " << minionRequestHandler.monitor->getHostname() << "\n";

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::MONITOR_MINION_PORT);

    int serverSocket;
    serverSocket = SocketUtils::createServerSocket(serverAddress);

    int minionSocket;
    while (true) {
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
            getsockname(minionSocket, (struct sockaddr*)&serverAddress, &len);
            inet_ntop(AF_INET, &serverAddress.sin_addr, ip, sizeof(ip));
            string minionAddress(ip);
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