#include "SysAdminRequestHandler.h"

SysAdminRequestHandler::SysAdminRequestHandler() {
    std::cout << "SysAdminRequestHandler created\n";
}

SysAdminRequestHandler::SysAdminRequestHandler(AuditingHub* auditingHub) : auditingHub{auditingHub} {
    std::cout << "SysAdminRequestHandler created with the name " << auditingHub->getHostname() << "\n";
}

void SysAdminRequestHandler::launchSessionProcess(){
    char* sshArgsStream;
    int size = asprintf(&sshArgsStream, "%s -i %s -oStrictHostKeyChecking=no -t %s@%s -vvvvv",
                        ProcessBinaries::SSH.c_str(), auditingHub->getHubKey().c_str(), auditingHub->getHubUserName().c_str(), remoteHost.c_str());
    string sshArgsStreamStr(sshArgsStream);
    std::vector<std::string> sshArgsStreamVec = General::splitString(sshArgsStreamStr);
    char* sshArgsStreamCharVec[sshArgsStreamVec.size()];
    int i = 0;
    for (const std::string& str : sshArgsStreamVec) {
        sshArgsStreamCharVec[i] = const_cast<char*>(str.c_str());
        i++;
    }
    sshArgsStreamCharVec[i] = NULL;

    if (DebugFlags::debugMonitor)
        cout << "Executing command: " << sshArgsStream << "\n";
    fflush(NULL);
    int pProcessWrite[2]; 
    int pProcessRead[2]; 
    pid_t pid = fork();
    if (pid == 0) {
        close(pProcessWrite[1]);
        close(pProcessRead[0]);
        int result = execvp(ProcessBinaries::SSH.c_str(), sshArgsStreamCharVec);
        if (result == -1) {
            if (DebugFlags::debugAuditingHub){
                cout << "Command failed\n";
                cout << "Result: " << result << "\n";
                cout << strerror(errno) << "\n";   
            }
            exit(-1);
        }
        exit(0);
    } else {
        close(pProcessWrite[0]);
        close(pProcessRead[1]);
    }

    if (pid > 0) {
        SysAdminRequestHandler::pidAuditingHubToNodeSessionProcess = pid;
        // SysAdminRequestHandler::processWrite = &pProcessWrite; //TODO
        // SysAdminRequestHandler::processRead = &pProcessRead;
        pipe(processWrite);
        pipe(processRead);
    }
}

void SysAdminRequestHandler::launchLogger(){
    //TODO
}

bool SysAdminRequestHandler::setNodeUntrusted(){
    struct hostent* serverHost;
    serverHost = SocketUtils::getHostByName(auditingHub->getMonitorHost());
    
    sockaddr_in monitorAddress;
    monitorAddress = SocketUtils::createServerAddress(Ports::MONITOR_AHUB_PORT);
    bcopy((char *)serverHost->h_addr, (char *)&monitorAddress.sin_addr.s_addr, serverHost->h_length);
    // int serverSocket;
    // serverSocket = SocketUtils::createServerSocket(serverAddress);
    int monitorSocket = socket(AF_INET, SOCK_STREAM, 0);
    SocketUtils::connectToServerSocket(monitorSocket, monitorAddress);
    if (DebugFlags::debugAuditor)
        cout << "Connected to the server\n";

    hostent* rHost = SocketUtils::getHostByName(this->remoteHost);
    struct in_addr addr;
     memcpy(&addr, rHost->h_addr_list[0], sizeof(struct in_addr));
     string host = inet_ntoa(addr);
     if (DebugFlags::debugAuditor)
         cout << "Host: " << host << "\n";
     char buffer[SocketUtils::MESSAGE_BYTES];
     std::string configuration = Messages::SET_UNTRUSTED + " " + host.c_str();
     General::stringToCharArray(configuration, buffer, SocketUtils::MESSAGE_BYTES);
     SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
     if (DebugFlags::debugAuditingHub)
         cout << "Wrote: " << buffer << " to Monitor\n";

     bzero(buffer, SocketUtils::MESSAGE_BYTES);
     SocketUtils::receiveBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
     if (DebugFlags::debugAuditingHub)
         cout << "Recieved: " << buffer << "\n";

     string result(buffer);
     vector<string> resultSplit = General::splitString(result);

     if (resultSplit[0] == Messages::NOT_OK) {
         if (DebugFlags::debugAuditingHub)
             cout << "Unsucessfull migration\n";
         close(monitorSocket);
         return false;
    } else if (resultSplit[0] == Messages::OK) {
        if (DebugFlags::debugAuditingHub)
            cout << "Sucessfull migration\n";
        close(monitorSocket);
        return true;
    }
    if (DebugFlags::debugAuditingHub)
        cout << "Unknown command about migration\n";
    close(monitorSocket);
    return false;
}

bool SysAdminRequestHandler::purgeMinion(){
    struct hostent* serverHost;
    serverHost = SocketUtils::getHostByName(auditingHub->getMonitorHost());

    sockaddr_in monitorAddress;
    monitorAddress = SocketUtils::createServerAddress(Ports::MONITOR_AHUB_PORT);
    bcopy((char *)serverHost->h_addr, (char *)&monitorAddress.sin_addr.s_addr, serverHost->h_length);
    // int serverSocket;
    // serverSocket = SocketUtils::createServerSocket(serverAddress);
    int monitorSocket = socket(AF_INET, SOCK_STREAM, 0);
    SocketUtils::connectToServerSocket(monitorSocket, monitorAddress);
    if (DebugFlags::debugAuditor)
        cout << "Connected to the server\n";

    hostent* rHost = SocketUtils::getHostByName(this->remoteHost);

    char buffer[SocketUtils::MESSAGE_BYTES];
    std::string configuration = Messages::PURGE;
    General::stringToCharArray(configuration, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugAuditingHub)
        cout << "Wrote: " << buffer << " to Monitor\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugAuditingHub)
        cout << "Recieved: " << buffer << "\n";

    string result(buffer);
    vector<string> resultSplit = General::splitString(result);

    if (resultSplit[0] == Messages::NOT_OK) {
        if (DebugFlags::debugAuditingHub)
            cout << "Unsucessfull purge\n";
        close(monitorSocket);
        return false;
    } else if (resultSplit[0] == Messages::OK) {
        if (DebugFlags::debugAuditingHub)
            cout << "Sucessfull purge\n";
        close(monitorSocket);
        return true;
    }
    if (DebugFlags::debugAuditingHub)
        cout << "Unknown command about purge\n";
    close(monitorSocket);
    return false;
}

bool SysAdminRequestHandler::launchManagementSession(){
    if (DebugFlags::debugAuditingHub)
        cout << "Will set and purge stuff.\n";
    if(!setNodeUntrusted())

    if(!purgeMinion())

    launchSessionProcess();
    launchLogger();

    if (DebugFlags::debugAuditingHub)
        cout << "Session created. Sending prompt to admin.\n";

    string promptString = "[" + adminUsername + "@" + remoteHost + "]>";
    //TODO  
    string x = string("echo ") + string("\"") + promptString + string("\"\n");

    string hostInput;
    string hostOutput;
    char buffer[SocketUtils::MESSAGE_BYTES];
    while(true){
        hostOutput = "xxx\n"; //READ from process
        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        General::stringToCharArray(hostOutput, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(adminToHubSocket, buffer, strlen(buffer), 0);

        if (hostOutput != promptString)
            continue;

        //LOG "Host->Admin:\n"+hostCompleteResponseBuilder.toString()

        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::receiveBuffer(adminToHubSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
        hostInput = string(buffer);
        if (hostInput == Messages::MANAGE_TEARDOWN){
            //auditingHubToNodeSessionProcess destroy process
            auditingHub->removeSession(remoteHost);
        }
        //xx = hostInput //WRITE to process

        //LOG "Admin->Host:"+hostInput+System.lineSeparator()
    }
}

void SysAdminRequestHandler::processAttestation(int adminSocket) {
    char buffer[SocketUtils::MESSAGE_BYTES];
    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(adminSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugAuditingHub)
        cout << "Recieved: " << buffer << "\n";

    string request(buffer);
    vector<string> requestSplit = General::splitString(request);
    if (requestSplit[0] == Messages::ATTEST) {
        if (DebugFlags::debugAuditingHub)
            cout << "Being attested\n";

        std::string quote = string(Messages::QUOTE) + " " + string(AttestationConstants::QUOTE) + " " + string(auditingHub->getApprovedSHA1()) + " " +  string(auditingHub->getApprovedSHA1());
        General::stringToCharArray(quote, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(adminSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugAuditingHub)
            cout << "Wrote: " << buffer << " to client\n";
    } else 
        throw 10; //TODO

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(adminSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugAuditingHub)
        cout << "Recieved: " << buffer << "\n";

    string response(buffer);
    vector<string> responseSplit = General::splitString(response);
    if (responseSplit[0] == Messages::NOT_OK) {
        throw 10;  //TODO
    }
}

void SysAdminRequestHandler::startSysAdminRequestHandler(SysAdminRequestHandler sysAdminRequestHandler, int adminToHubSocket) {
    std::cout << "SysAdminRequestHandler running with Auditing Hub with the name " << sysAdminRequestHandler.auditingHub->getHostname() << "\n";

    sysAdminRequestHandler.processAttestation(adminToHubSocket);

    // sockaddr_in serverAddress;
    // serverAddress = SocketUtils::createServerAddress(Ports::AHUB_AUDITOR_PORT);

    // int serverSocket;
    // serverSocket = SocketUtils::createServerSocket(serverAddress);

    // int clientSocket;
    while (true) {
        // clientSocket = SocketUtils::acceptClientSocket(serverSocket);
        // cout << "Got connection from client\n";

        char buffer[SocketUtils::MESSAGE_BYTES];
        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::receiveBuffer(adminToHubSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);

        if (DebugFlags::debugAuditingHub)
            cout << "Recieved: " << buffer << "\n";

        string command(buffer);
        vector<string> commandSplit = General::splitString(command);

        if (commandSplit[0] != Messages::MANAGE) {
            std::string response = Messages::NOT_OK;
            bzero(buffer, SocketUtils::MESSAGE_BYTES);
            General::stringToCharArray(response, buffer, SocketUtils::MESSAGE_BYTES);
            SocketUtils::sendBuffer(adminToHubSocket, buffer, strlen(buffer), 0);
            if (DebugFlags::debugAuditingHub)
                cout << "Wrote: " << buffer << " to client\n";
            return;
        }

        sysAdminRequestHandler.adminUsername = commandSplit[1];
        sysAdminRequestHandler.remoteHost = commandSplit[2];

        bool launchManagementSessionResult = false;
        if (sysAdminRequestHandler.auditingHub->checkPermissionAndUnqueue(sysAdminRequestHandler.remoteHost)) {
            std::string response = Messages::OK;
            bzero(buffer, SocketUtils::MESSAGE_BYTES);
            General::stringToCharArray(response, buffer, SocketUtils::MESSAGE_BYTES);
            SocketUtils::sendBuffer(adminToHubSocket, buffer, strlen(buffer), 0);
            if (DebugFlags::debugAuditingHub)
                cout << "Wrote: " << buffer << " to client\n";
        }

        launchManagementSessionResult = sysAdminRequestHandler.launchManagementSession();
        if (launchManagementSessionResult){
            std::string response = Messages::OK;
            bzero(buffer, SocketUtils::MESSAGE_BYTES);
            General::stringToCharArray(response, buffer, SocketUtils::MESSAGE_BYTES);
            SocketUtils::sendBuffer(adminToHubSocket, buffer, strlen(buffer), 0);
            if (DebugFlags::debugAuditingHub)
                cout << "Wrote: " << buffer << " to client\n";
        } else {
            std::string response = Messages::NOT_OK;
            bzero(buffer, SocketUtils::MESSAGE_BYTES);
            General::stringToCharArray(response, buffer, SocketUtils::MESSAGE_BYTES);
            SocketUtils::sendBuffer(adminToHubSocket, buffer, strlen(buffer), 0);
            if (DebugFlags::debugAuditingHub)
                cout << "Wrote: " << buffer << " to client\n";
        }
    }
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create SysAdminRequestHandler\n";

//     SysAdminRequestHandler* sysAdminRequestHandler;
//     sysAdminRequestHandler = new SysAdminRequestHandler();

//     std::cout << "Bubye\n";

// }