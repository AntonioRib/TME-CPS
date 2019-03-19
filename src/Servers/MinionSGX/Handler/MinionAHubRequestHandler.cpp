#include "MinionAHubRequestHandler.h"

MinionAHubRequestHandler::MinionAHubRequestHandler() {
    std::cout << "AHubRequestHandler created\n";
}

MinionAHubRequestHandler::MinionAHubRequestHandler(Minion* minion) : minion{minion} {
    std::cout << "AHubRequestHandler created with the name " << minion->getIpAddress() << "\n";
}

bool MinionAHubRequestHandler::purgeMinion() {
    char* purgeArgsStream;
    int size = asprintf(&purgeArgsStream, "sudo ./%s", Scripts::PURGE_MINION.c_str());
    string purgeArgsStreamStr(purgeArgsStream);
    std::vector<std::string> purgeArgsStreamVec = General::splitString(purgeArgsStreamStr);
    char* purgeArgsStreamCharVec[purgeArgsStreamVec.size()];
    int i = 0;
    for (const std::string& str : purgeArgsStreamVec) {
        purgeArgsStreamCharVec[i] = const_cast<char*>(str.c_str());
        i++;
    }
    purgeArgsStreamCharVec[i] = NULL;


    if (DebugFlags::debugMonitor)
        cout << "Executing command: " << purgeArgsStream << "\n";
    fflush(NULL);
    pid_t pid = fork();
    if (pid == 0) {
        int result = execvp(purgeArgsStreamCharVec[0], purgeArgsStreamCharVec);
        if (result == -1) {
            if (DebugFlags::debugMinion){
                cout << "Command failed\n";
                cout << "Result: " << result << "\n";
                cout << strerror(errno) << "\n";   
            }
            exit(-1);
        }
        exit(0);
    }

    if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        if (WEXITSTATUS(status) == -1)
            return false;
    }
    return true;
}

void MinionAHubRequestHandler::startMinionAHubRequestHandler(MinionAHubRequestHandler* minionAHubRequestHandler) {
    std::cout << "MinionAHubRequestHandler running with Minion with the ip " << minionAHubRequestHandler->minion->getIpAddress() << "\n";

    sockaddr_in minionAddress;
    minionAddress = SocketUtils::createServerAddress(Ports::MINION_AHUB_PORT);

    int minionSocket;
    minionSocket = SocketUtils::createServerSocket(minionAddress);

    if (DebugFlags::debugMinion)
        cout << "MinionAHubRequestHandler Created socket to Monitor\n";

    int aHubSocket;
    while (true) {
        cout << "Waiting for connections...\n";
        try {
            if (DebugFlags::debugMinion)
                cout << "MinionAHubRequestHandler Waiting for connection\n";
            aHubSocket = SocketUtils::acceptClientSocket(minionSocket);
            cout << "Got connection from AuditingHub\n";

            char buffer[SocketUtils::MESSAGE_BYTES];
            bzero(buffer, SocketUtils::MESSAGE_BYTES);
            SocketUtils::receiveBuffer(aHubSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
            if (DebugFlags::debugMonitor)
                cout << "Recieved: " << buffer << "\n";
            // cout << buffer;
            string command(buffer);
            vector<string> commandSplit = General::splitString(command);

            bool requestResult = false;
            if (commandSplit[0] == Messages::PURGE) {
                requestResult = minionAHubRequestHandler->purgeMinion();
            }

            if (requestResult) {
                bzero(buffer, SocketUtils::MESSAGE_BYTES);
                std::string result = Messages::OK;
                General::stringToCharArray(result, buffer, SocketUtils::MESSAGE_BYTES);
                SocketUtils::sendBuffer(aHubSocket, buffer, strlen(buffer), 0);
                if (DebugFlags::debugMonitor)
                    cout << "Success \n";
            } else {
                bzero(buffer, SocketUtils::MESSAGE_BYTES);
                std::string result = Messages::NOT_OK;
                General::stringToCharArray(result, buffer, SocketUtils::MESSAGE_BYTES);
                SocketUtils::sendBuffer(aHubSocket, buffer, strlen(buffer), 0);
                if (DebugFlags::debugMonitor)
                    cout << "Failed \n";
            }
        } catch (const exception& e){
            close(aHubSocket);
            cout << e.what() << '\n';
        }
    }
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create AHubRequestHandler\n";

//     AHubRequestHandler* ahubRequestHandler;
//     ahubRequestHandler = new AHubRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }