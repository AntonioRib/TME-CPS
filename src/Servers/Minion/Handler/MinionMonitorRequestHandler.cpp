#include "MinionMonitorRequestHandler.h"
using namespace std;

MinionMonitorRequestHandler::MinionMonitorRequestHandler() {
    std::cout << "MonitorRequestHandler created\n";
}

MinionMonitorRequestHandler::MinionMonitorRequestHandler(Minion* minion) : minion{minion} {
    std::cout << "MonitorRequestHandler created with the name " << minion->getIpAddress() << "\n";
}

bool MinionMonitorRequestHandler::deployApp(string appId) {
    char* createContainerArgsStream;
    int sizeCreateStream = asprintf(&createContainerArgsStream, "sudo docker build -t %s-container ../../%s%s", appId.c_str(), Directories::APPS_DIR_MINION.c_str(), appId.c_str());

    char* deployContainerArgsStream;
    int sizeDeployStream = asprintf(&deployContainerArgsStream, "sudo docker run -p 80 -d --name %s %s-container", appId.c_str(), appId.c_str());

    if (DebugFlags::debugMonitor)
        cout << "Executing command: " << createContainerArgsStream << "\n";
    fflush(NULL);
    pid_t pidCreate = fork();
    if (pidCreate == 0) {
        int result = execlp(createContainerArgsStream, createContainerArgsStream);
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

    if (pidCreate > 0) {
        int status;
        waitpid(pidCreate, &status, 0);
        if (WEXITSTATUS(status) == -1)
            return false;
    }

    if (DebugFlags::debugMonitor)
        cout << "Executing command: " << deployContainerArgsStream << "\n";
    fflush(NULL);
    pid_t pidDeploy = fork();
    if (pidDeploy == 0) {
        int result = execlp(deployContainerArgsStream, deployContainerArgsStream);
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

    if (pidDeploy > 0) {
        int status;
        waitpid(pidDeploy, &status, 0);
        if (WEXITSTATUS(status) == -1)
            return false;
    }

    return true;
}

bool MinionMonitorRequestHandler::deleteApp(string appId) {
    char* deleteArgsStream;
    int size = asprintf(&deleteArgsStream, "sudo ../%s", Scripts::DELETE_APP.c_str());

    if (DebugFlags::debugMonitor)
        cout << "Executing command: " << deleteArgsStream << "\n";
    fflush(NULL);
    pid_t pid = fork();
    if (pid == 0) {
        int result = execlp(deleteArgsStream, deleteArgsStream);
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

void MinionMonitorRequestHandler::processAttestation(int monitorSocket, string nonce) { 
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
        throw 10;  //TODO
    } else if (approvedSplit[0] == Messages::OK_APPROVED) {
        cout << "Approved!\n";
        // cout << "Configuration approved. Auditor signature for monitor: " + approvedSplit[1] + ". For minions:" + approvedSplit[3];
    }
}

void MinionMonitorRequestHandler::startMinionMonitorRequestHandler(MinionMonitorRequestHandler* minionMonitorRequestHandler) {
    std::cout << "MinionMonitorRequestHandler running with Minion with the name " << minionMonitorRequestHandler->minion->getIpAddress() << "\n";

    sockaddr_in minionAddress;
    minionAddress = SocketUtils::createServerAddress(Ports::MINION_MONITOR_PORT);

    int minionSocket;
    minionSocket = SocketUtils::createServerSocket(minionAddress);

    if (DebugFlags::debugMinion)
        cout << "MinionMonitorRequestHandler Created socket to Monitor\n";

    int monitorSocket;
    while (true) {
        if (DebugFlags::debugMinion)
            cout << "MinionMonitorRequestHandler Waiting for connection\n";
        monitorSocket = SocketUtils::acceptClientSocket(minionSocket);
        cout << "Got connection from Monitor\n";

        char buffer[SocketUtils::MESSAGE_BYTES];
        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::receiveBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
        if (DebugFlags::debugMinion)
            cout << "Recieved: " << buffer << "\n";
        // cout << buffer;
        string command(buffer);
        vector<string> commandSplit = General::splitString(command);

        bool requestResult = false;
        if (commandSplit[0] == Messages::DEPLOY) {
            if (DebugFlags::debugMinion)
                cout << "Deploying " << commandSplit[1] << "\n";
            requestResult = minionMonitorRequestHandler->deployApp(commandSplit[1]);
        } else if (commandSplit[0] == Messages::DELETE_APP) {
            if (DebugFlags::debugMinion)
                cout << "Deleting " << commandSplit[1] << "\n";
            requestResult = minionMonitorRequestHandler->deleteApp(commandSplit[1]);
        } else if (commandSplit[0] == Messages::ATTEST) {
            minionMonitorRequestHandler->processAttestation(monitorSocket, commandSplit[1]);
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
    }
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create MonitorRequestHandler\n";

//     MonitorRequestHandler* monitorRequestHandler;
//     monitorRequestHandler = new MonitorRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }