#include "Developer.h"
using namespace std;

const int REQUEST_FLAG_INDEX = 1;
const int MONITOR_HOST_FLAG_INDEX = 2;
const int USERNAME_FLAG_INDEX = 4;
const int KEY_FLAG_INDEX = 6;
const int APPDIR_FLAG_INDEX = 8;
const int INSTANCES_FLAG_INDEX = 10;


//  Usage:
//  Developer -h: help
//  Developer

Developer::Developer() {
    std::cout << "Developer created\n";
}

Developer::Developer(string monitorHost, string username, string key, string appDir, int instances) {
    std::cout << "Developer created\n";
}

Developer::Developer(string monitorHost, string username, string key, string appDir) {
    std::cout << "Developer created\n";
}

void Developer::attestMonitor(int monitorSocket){
    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::MONITOR_DEVELOPER_PORT);

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    SocketUtils::connectToServerSocket(clientSocket, serverAddress);
    if (DebugFlags::debugDeveloper)
        cout << "Connected to the server\n";

    char buffer[SocketUtils::MESSAGE_BYTES];
    string attestationRequestString = Messages::ATTEST + " " + AttestationConstants::NONCE;
    General::stringToCharArray(attestationRequestString, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(clientSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugDeveloper)
        cout << "Wrote: " << buffer << " to server\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(clientSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugDeveloper)
        cout << "Recieved from server: " << buffer << "\n";

    string quote(buffer);
    vector<string> splittedQuote = General::splitString(quote);

    if (splittedQuote[0] == Messages::QUOTE && splittedQuote[1] == AttestationConstants::QUOTE && splittedQuote[2] == AttestationConstants::PCR_SHA1 && splittedQuote[3] == AttestationConstants::PCR_SHA1) {
        string approvedMessage = Messages::OK_APPROVED;
        General::stringToCharArray(approvedMessage, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(clientSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugDeveloper)
            cout << "Wrote: " << buffer << " to server\n";
    } else {
        General::stringToCharArray(Messages::NOT_APPROVED, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(clientSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugDeveloper)
            cout << "Wrote: " << buffer << " to server\n";
    }
}

bool Developer::sendSyncMessageAndGetResponse(string message){
    // struct hostent* serverHost;
    // serverHost = SocketUtils::getHostByName(hostname);

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::MONITOR_DEVELOPER_PORT);

    int monitorSocket = socket(AF_INET, SOCK_STREAM, 0);

    SocketUtils::connectToServerSocket(monitorSocket, serverAddress);
    if (DebugFlags::debugDeveloper)
        cout << "Connected to the server\n";

    attestMonitor(monitorSocket);

    bool sendResult = true;
    vector<string> splittedMessage = General::splitString(message);
    if (splittedMessage[0] == Messages::NEW_APP){
        sendResult = Developer::sendApp();
    }

    if (!sendResult)
        return sendResult;

    char buffer[SocketUtils::MESSAGE_BYTES];
    General::stringToCharArray(message, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugDeveloper)
        cout << "Wrote: " << buffer << " to server\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugDeveloper)
        cout << "Recieved from server: " << buffer << "\n";

    string response(buffer);
    vector<string> splittedResponse = General::splitString(response);

    if (splittedResponse[0] == Messages::OK) {
        close(monitorSocket);
        return true;
    } else if (splittedResponse[0] == Messages::NOT_OK) {
        close(monitorSocket);
        return false;
    }
    close(monitorSocket);
    return false;
}

bool Developer::deleteApp(){
    if (DebugFlags::debugDeveloper)
        cout << "Deleting app on monitor\n";

    string message = Messages::DELETE_APP + " " + Developer::username + " " + appDir.substr(appDir.find("/")+1);
    bool messageResult;
    messageResult = sendSyncMessageAndGetResponse(message);

    if (!messageResult)
        return false;
    return true;
}

bool Developer::sendApp(){
    char* scpArgsStream;
    int size = asprintf(&scpArgsStream, "%s -r -i %s -oStrictHostKeyChecking=no %s %s@%s:%s%s",
                        ProcessBinaries::SCP.c_str(), key.c_str(), appDir.c_str(), username.c_str(),
                        monitorHost.c_str(), Directories::APPS_DIR_MONITOR.c_str(), appDir.substr(appDir.find("/")+1).c_str());

    if (DebugFlags::debugDeveloper)
        cout << "Executing command: " << scpArgsStream << "\n";
    fflush(NULL);
    pid_t pid = fork();
    if (pid == 0) {
        int result = execlp(ProcessBinaries::SCP.c_str(), scpArgsStream);
        if (result == -1) {
            if (DebugFlags::debugDeveloper)
                cout << "Command failed\n";
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

bool Developer::deployApp(){
    if (DebugFlags::debugDeveloper)
        cout << "Deploying app on monitor\n";

    string message = Messages::NEW_APP + " " + Developer::username + " " + appDir.substr(appDir.find("/") + 1) + " " + to_string(Developer::instances);
    bool messageResult;
    messageResult = sendSyncMessageAndGetResponse(message);

    if (!messageResult)
        return false;
    return true;
}

void printHelp() {
    std::cout << "Usage to Deploy App: Developer -deploy -m monitorHost -u username -k key -a appDir -i #instances\n";
    std::cout << "Usage to Delete App: Developer -delete -m monitorHost -u username -k key -a appDir \n";
}

int main(int argc, char* argv[]) {
    string monitorHost;
    string username;
    string key;
    string appDir;
    string instances;
    Developer* developer;

    if (argc != 10 && argc != 12) {
        std::cout << "Invalid number of fields\n";
        printHelp();
        return -1;
    }
    string command = argv[REQUEST_FLAG_INDEX];
    bool commandResult = false;
    if(command == "-deploy"){
        monitorHost = argv[MONITOR_HOST_FLAG_INDEX];
        username = argv[USERNAME_FLAG_INDEX];
        key = argv[KEY_FLAG_INDEX];
        appDir = argv[APPDIR_FLAG_INDEX];
        instances = argv[INSTANCES_FLAG_INDEX];
        developer = new Developer(monitorHost, username, key, appDir, atoi(instances.c_str()));
        commandResult = developer->deployApp();
    } else if (command == "-delete"){
        monitorHost = argv[MONITOR_HOST_FLAG_INDEX];
        username = argv[USERNAME_FLAG_INDEX];
        key = argv[KEY_FLAG_INDEX];
        appDir = argv[APPDIR_FLAG_INDEX];
        developer = new Developer(monitorHost, username, key, appDir);
        commandResult = developer->deleteApp();
    } else {
        std::cout << "Unknown Command\n";
        printHelp();
    }

    if(commandResult)
        cout << "Success";
    else 
        cout << "Failed";

    return 0;
}