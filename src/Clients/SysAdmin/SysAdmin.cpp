#include "SysAdmin.h"
using namespace std;

const int HUB_FLAG_INDEX = 1;
const int HOST_FLAG_INDEX = 3;
const int USERNAME_FLAG_INDEX = 5;
const int ADMIN_KEY_INDEX = 7;

const int ERROR_CODE = 255;

//  Usage:
//  SysAdmin -h: help
//  SysAdmin

SysAdmin::SysAdmin() {
    std::cout << "SysAdmin created\n";
}

SysAdmin::SysAdmin(string username, string hubhost, string remoteHost, string key) : username{username}, hubHost{hubhost}, remoteHost{remoteHost}, key{key} {
    std::cout << "SysAdmin created\n";
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
        bzero(buffer, SocketUtils::MESSAGE_BYTES);
    } else {
        General::stringToCharArray(Messages::NOT_APPROVED, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(loggerSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugSysAdmin)
            cout << "Wrote: " << buffer << " to server\n";
        bzero(buffer, SocketUtils::MESSAGE_BYTES);
    }
}

bool SysAdmin::startLocalProxy() {
    char* sshArgsStream;
    int size = asprintf(&sshArgsStream, "%s -i %s -oStrictHostKeyChecking=no -f %s@%s -L %d:%s:%d -N",
                        ProcessBinaries::SSH.c_str(), key.c_str(), username.c_str(), hubHost.c_str(), 
                        Ports::ADMIN_SSH_PORT, hubHost.c_str(), Ports::AHUB_LOCAL_PORT);
    // int size = asprintf(&sshArgsStream, "%s -i %s -oStrictHostKeyChecking=no %s",
    //                     ProcessBinaries::SSH.c_str(), key.c_str(), hubHost.c_str());
    string sshArgsStreamStr(sshArgsStream);
    std::vector<std::string> sshArgsStreamVec = General::splitString(sshArgsStreamStr);
    char* sshArgsStreamCharVec[sshArgsStreamVec.size()];
    int i = 0;
    for (const std::string& str : sshArgsStreamVec) {
        sshArgsStreamCharVec[i] = const_cast<char*>(str.c_str());
        i++;
    }
    sshArgsStreamCharVec[i] = NULL;
    if (DebugFlags::debugSysAdmin)
        cout << "Executing command: " << sshArgsStream << "\n";
    fflush(NULL);
    pid_t pid = fork();
    if (pid == 0) {
        int result = 0;
        result = execvp(ProcessBinaries::SSH.c_str(), sshArgsStreamCharVec);
        if (result == -1) {
            if (DebugFlags::debugSysAdmin){
                cout << "Command failed\n";
                cout << "Result: "<< result<< "\n";
                cout << strerror(errno) << "\n";
            }
                exit(-1);
        }
        if (DebugFlags::debugSysAdmin)
            cout << "Command suceeded\n";
         exit(0);
    }

    if (pid > 0) {
        int status = 0;
        if (DebugFlags::debugSysAdmin)
            cout << "Waiting for child process\n";
            waitpid(pid, &status, 0);
            if (WIFEXITED(status) && WEXITSTATUS(status) == ERROR_CODE) {
                if (DebugFlags::debugSysAdmin)
                    cout << "Child process returned bad\n";
                return false;
            }

    }
    if (DebugFlags::debugSysAdmin)
        cout << "Child process returned good\n";
    return true;
}

bool SysAdmin::manageNode(){
    bool proxyCreationResult = SysAdmin::startLocalProxy();
    
    if (!proxyCreationResult) 
        return false;
        
    struct hostent* serverHost;
    serverHost = SocketUtils::getHostByName(hubHost);

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::AHUB_SYSADMIN_PORT+20);
    bcopy((char *)serverHost->h_addr, (char *)&serverAddress.sin_addr.s_addr, serverHost->h_length);
    int hubSocket = socket(AF_INET, SOCK_STREAM, 0);
    SocketUtils::connectToServerSocket(hubSocket, serverAddress);

    attestLogger(hubSocket);

    char buffer[SocketUtils::MESSAGE_BYTES];
    bzero(buffer, SocketUtils::MESSAGE_BYTES);
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
    
    string prompt = "[" + username + "@" + remoteHost + "]> ";

    while(true){
        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::receiveBuffer(hubSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);

        string hostOutput(buffer);
        vector<string> hostOutputQuote = General::splitString(hostOutput);
        if (responseQuote[0] == Messages::NOT_OK)
            return false;

        cout << buffer;
        fflush(NULL);
        // cout << "\n";

        if (hostOutput.find(prompt) == std::string::npos)
            continue;

        string hostinput;
        getline(cin, hostinput);

        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        General::stringToCharArray(hostinput, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(hubSocket, buffer, strlen(buffer), 0);
        
        if (hostinput == Messages::QUIT)
            break;
    }

    if (DebugFlags::debugSysAdmin)
        cout << "Wrote: " << buffer << " to server\n";
    return true;
}

bool SysAdmin::urgentManageNode(){
    bool proxyCreationResult = SysAdmin::startLocalProxy();
    
    if (!proxyCreationResult) 
        return false;
        
    struct hostent* serverHost;
    serverHost = SocketUtils::getHostByName(hubHost);

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::AHUB_SYSADMIN_PORT+20);
    bcopy((char *)serverHost->h_addr, (char *)&serverAddress.sin_addr.s_addr, serverHost->h_length);
    int hubSocket = socket(AF_INET, SOCK_STREAM, 0);
    SocketUtils::connectToServerSocket(hubSocket, serverAddress);

    attestLogger(hubSocket);

    char buffer[SocketUtils::MESSAGE_BYTES];
    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    string requestString = Messages::URGENTMANAGE + " " + username + " " + remoteHost;
    General::stringToCharArray(requestString, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(hubSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugSysAdmin)
        cout << "Wrote: " << buffer << " to server\n";

    // bzero(buffer, SocketUtils::MESSAGE_BYTES);
    // SocketUtils::receiveBuffer(hubSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    // if (DebugFlags::debugSysAdmin)
    //     cout << "Recieved from server: " << buffer << "\n";

    // string response(buffer);
    // vector<string> responseQuote = General::splitString(response);
    // if(responseQuote[0] == Messages::NOT_OK)
    //     return false;

    // cout << buffer;

    string prompt = "[" + username + "@" + remoteHost + "]> ";

    while(true){
        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::receiveBuffer(hubSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
        if (DebugFlags::debugSysAdmin)
            cout << "Recieved from server: " << buffer << "\n";

        string hostOutput(buffer);
        vector<string> hostOutputQuote = General::splitString(hostOutput);
        if (hostOutputQuote[0] == Messages::NOT_OK)
            return false;

        cout << hostOutput;
        fflush(NULL);
        // cout << "\n";

        if (hostOutput.find(prompt) == std::string::npos)
            continue;

        string hostinput;
        getline(cin, hostinput);

        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        General::stringToCharArray(hostinput, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(hubSocket, buffer, strlen(buffer), 0);
        
        if (hostinput == Messages::QUIT)
            break;
    }

    if (DebugFlags::debugSysAdmin)
        cout << "Wrote: " << buffer << " to server\n";
    return true;
}

void printHelp() {
    std::cout << "Usage: SysAdmin -a hub -h host -u username -k key\n";
    std::cout << "Usage: SysAdmin -a hub -h host -u username -k key -force\n";
}

int main(int argc, char* argv[]) {
    if (argc != 9 && argc != 10) {
        std::cout << "Invalid number of fields\n";
        printHelp();
        return -1;
    }

    string hubHost = argv[HUB_FLAG_INDEX+1];
    string remoteHost = argv[HOST_FLAG_INDEX+1];
    string username = argv[USERNAME_FLAG_INDEX + 1];
    string key = argv[ADMIN_KEY_INDEX+1];
    SysAdmin* sysAdmin;
    sysAdmin = new SysAdmin(username, hubHost, remoteHost, key);
    bool commandResult = false;

    if(argc == 9)
        commandResult = sysAdmin->manageNode();
    else if(argc == 10 ){
        commandResult = sysAdmin->urgentManageNode();
    }

    if (!commandResult) {
         cout << "Failed to perform the request \n";   
    }
    return 0;
}
