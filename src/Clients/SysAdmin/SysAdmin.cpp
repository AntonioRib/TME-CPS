#include "SysAdmin.h"
using namespace std;

const int HUB_FLAG_INDEX = 1;
const int HOST_FLAG_INDEX = 3;
const int USERNAME_FLAG_INDEX = 5;
const int ADMIN_KEY_INDEX = 7;

//  Usage:
//  SysAdmin -h: help
//  SysAdmin

SysAdmin::SysAdmin() {
    std::cout << "Auditor created\n";
}

SysAdmin::SysAdmin(string username, string hubhost, string remoteHost, string key) {
    std::cout << "Auditor created\n";
}

void SysAdmin::attestLogger(int loggerSocket){
    char buffer[SocketUtils::MESSAGE_BYTES];
    string attestationRequestString = Messages::ATTEST + " " + AttestationConstants::NONCE;
    General::stringToCharArray(attestationRequestString, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(loggerSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugSysAdmin)
        cout << "Wrote: " << buffer << " to server\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(loggerSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugSysAdmin)
        cout << "Recieved from server: " << buffer << "\n";

    string quote(buffer);
    vector<string> splittedQuote = General::splitString(quote);

    if (splittedQuote[0] == Messages::QUOTE && splittedQuote[1] == AttestationConstants::QUOTE && splittedQuote[2] == AttestationConstants::PCR_SHA1 && splittedQuote[3] == AttestationConstants::PCR_SHA1) {
        string approvedMessage = Messages::OK_APPROVED;
        General::stringToCharArray(approvedMessage, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(loggerSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugSysAdmin)
            cout << "Wrote: " << buffer << " to server\n";
    } else {
        General::stringToCharArray(Messages::NOT_APPROVED, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(loggerSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugSysAdmin)
            cout << "Wrote: " << buffer << " to server\n";
    }
}

bool SysAdmin::startLocalProxy() {
    char* sshArgsStream;
    int size = asprintf(&sshArgsStream, "%s -i %s -oStrictHostKeyChecking=no -f %s@%s -L %d:%s:%d -N",
                        ProcessBinaries::SSH.c_str(), key.c_str(), username.c_str(), hubHost.c_str(), 
                        Ports::ADMIN_SSH_PORT, hubHost.c_str(), Ports::AHUB_LOCAL_PORT);

    if (DebugFlags::debugSysAdmin)
        cout << "Executing command: " << sshArgsStream << "\n";
    fflush(NULL);
    pid_t pid = fork();
    if (pid == 0) {
        int result = execlp(ProcessBinaries::SSH.c_str(), sshArgsStream);
        if (result == -1) {
            if (DebugFlags::debugSysAdmin)
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

bool SysAdmin::manageNode(){
    bool proxyCreationResult = SysAdmin::startLocalProxy();

    if (!proxyCreationResult)
        return false;

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::ADMIN_SSH_PORT);

    int hubSocket = socket(AF_INET, SOCK_STREAM, 0);
    SocketUtils::connectToServerSocket(hubSocket, serverAddress);

    attestLogger(hubSocket);

    char buffer[SocketUtils::MESSAGE_BYTES];
    string requestString = Messages::MANAGE + " " + username + " " + remoteHost;
    General::stringToCharArray(requestString, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(hubSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugSysAdmin)
        cout << "Wrote: " << buffer << " to server\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(hubSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugSysAdmin)
        cout << "Recieved from server: " << buffer << "\n";

    string response(buffer);
    vector<string> responseQuote = General::splitString(response);
    if(responseQuote[0] == Messages::NOT_OK)
        return false;

    string prompt = "[" + username + "@" + remoteHost + "]>";

    while(true){
        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::receiveBuffer(hubSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
        if (DebugFlags::debugSysAdmin)
            cout << "Recieved from server: " << buffer << "\n";

        string hostOutput(buffer);
        vector<string> hostOutputQuote = General::splitString(hostOutput);
        if (responseQuote[0] == Messages::NOT_OK)
            return false;
        cout << buffer;
        fflush(NULL);

        if (hostOutput != prompt)
            continue;

        string hostinput;
        getline(cin, hostinput);
        if (hostinput == Messages::QUIT)
            break;

        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        General::stringToCharArray(hostinput, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(hubSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugSysAdmin)
            cout << "Wrote: " << buffer << " to server\n";
    }

    if (DebugFlags::debugSysAdmin)
        cout << "Wrote: " << buffer << " to server\n";
    return true;
}

void printHelp() {
    std::cout << "Usage: SysAdmin -a hub -h host -u username -k key\n";
}

int main(int argc, char* argv[]) {
    if (argc != 9) {
        std::cout << "Invalid number of fields\n";
        printHelp();
        return -1;
    }

    string hubHost = argv[HUB_FLAG_INDEX+1];
    string remoteHost = argv[HOST_FLAG_INDEX+1];
    string username = argv[HUB_FLAG_INDEX+1];
    string key = argv[ADMIN_KEY_INDEX+1];
    SysAdmin* sysAdmin;
    sysAdmin = new SysAdmin(username, hubHost, remoteHost, key);
    bool commandResult = false;
    commandResult = sysAdmin->manageNode();

    if (!commandResult) {
         cout << "Failed to perform the request \n";   
    }
    return 0;
}
