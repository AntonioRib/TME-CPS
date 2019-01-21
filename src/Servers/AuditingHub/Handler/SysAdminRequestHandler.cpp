#include "SysAdminRequestHandler.h"

const string UNCOMMITED_LOGS_DIR = "Logs/Uncommited/";
const string COMMITED_LOGS_DIR = "Logs/Commited/";

SysAdminRequestHandler::SysAdminRequestHandler() {
    std::cout << "SysAdminRequestHandler created\n";
}

SysAdminRequestHandler::SysAdminRequestHandler(AuditingHub* auditingHub) : auditingHub{auditingHub} {
    std::cout << "SysAdminRequestHandler created with the name " << auditingHub->getHostname() << "\n";
}

void SysAdminRequestHandler::launchSessionProcess(){
    char* sshArgsStream;
    int size = asprintf(&sshArgsStream, "%s -i %s -oStrictHostKeyChecking=no -t %s@%s",
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
    if (DebugFlags::debugAuditingHub)
        cout << "Executing command: " << sshArgsStream << "\n";
    fflush(NULL);
    pipe(processWrite);
    pipe(processRead);
    pid_t pid = fork();
    if (pid == 0) {
        close(processWrite[1]);
        close(processRead[0]);
        
        dup2(processWrite[0], fileno(stdin));
        dup2(processRead[1], fileno(stdout));

        int result = execvp(ProcessBinaries::SSH.c_str(), sshArgsStreamCharVec);
        if (result == -1) {
            if (DebugFlags::debugAuditingHub){
                cout << "Command failed\n";
                cout << "Result: " << result << "\n";
                cout << strerror(errno) << "\n";   
            }
            exit(-1);
        }
        // exit(0);
    }

    if (pid > 0) {
        SysAdminRequestHandler::pidAuditingHubToNodeSessionProcess = pid;
    }
}

void SysAdminRequestHandler::launchLogger(){
    if (mkdir(UNCOMMITED_LOGS_DIR.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1)
        cout << "Directory created";

    string logPath(UNCOMMITED_LOGS_DIR + "AuditingHub" + "." + adminUsername + "." + remoteHost + "." + General::currentDateTime());
    SysAdminRequestHandler::logger = spdlog::basic_logger_mt("Logger", logPath);
    // logger->info("Logger started");
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
    if (DebugFlags::debugAuditingHub)
        cout << "Connected to the server\n";

    hostent* rHost = SocketUtils::getHostByName(this->remoteHost);
    struct in_addr addr;
     memcpy(&addr, rHost->h_addr_list[0], sizeof(struct in_addr));
     string host = inet_ntoa(addr);

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
    serverHost = SocketUtils::getHostByName(SysAdminRequestHandler::remoteHost);

    sockaddr_in monitorAddress;
    monitorAddress = SocketUtils::createServerAddress(Ports::MINION_AHUB_PORT);
    bcopy((char *)serverHost->h_addr, (char *)&monitorAddress.sin_addr.s_addr, serverHost->h_length);
    // int serverSocket;
    // serverSocket = SocketUtils::createServerSocket(serverAddress);
    int monitorSocket = socket(AF_INET, SOCK_STREAM, 0);
    SocketUtils::connectToServerSocket(monitorSocket, monitorAddress);
    if (DebugFlags::debugAuditingHub)
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
        return false;

    if(!purgeMinion())
        return false;
    
    launchSessionProcess();
    launchLogger();

    if (DebugFlags::debugAuditingHub)
        cout << "Session created. Sending prompt to admin.\n";

    string promptString = "[" + adminUsername + "@" + remoteHost + "]>";
    string promptStringCmd = string("echo ") + string("\"") + promptString + string("\"\n");
    write(processWrite[1], promptStringCmd.c_str(), strlen(promptStringCmd.c_str())+1);
    if (DebugFlags::debugAuditingHub)
        cout << "Wrote: " << promptStringCmd << " to Pipe\n";

    string hostInput;
    string hostOutput;
    char buffer[SocketUtils::MESSAGE_BYTES];
    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    while(true){
        int nbytes = read(processRead[0], buffer, sizeof(buffer));
        hostOutput = string(buffer); //READ from process
        // hostOutput = hostOutput + "\n";
        // if (DebugFlags::debugAuditingHub)
        //     cout << "Read: " << hostOutput << " from Pipe\n";
        
        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        General::stringToCharArray(hostOutput, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(adminToHubSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugAuditingHub)
            cout << "Wrote: " << buffer << " to Admin\n";
        if (DebugFlags::debugAuditingHub)
            cout << "hostOutput: " << hostOutput << "\n";
        if (DebugFlags::debugAuditingHub)
            cout << "promptString: " << promptString << "\n";
        // std::this_thread::sleep_for(std::chrono::seconds(1));
        if (hostOutput.substr(0, promptString.size()) != promptString)
            continue;

        SysAdminRequestHandler::logger->info("Host->Admin:\n"+hostOutput);
        //LOG "Host->Admin:\n"+hostCompleteResponseBuilder.toString()

        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::receiveBuffer(adminToHubSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
        hostInput = string(buffer);
        if (hostInput == Messages::MANAGE_TEARDOWN){
            if (DebugFlags::debugAuditingHub)
                cout << "Session has ended\n";
            //auditingHubToNodeSessionProcess destroy process
            auditingHub->removeSession(remoteHost);
            return true;
        }
        // string promptString = "[" + adminUsername + "@" + remoteHost + "]>";
        string response = hostInput + string("; echo ") + string("\"") + promptString + string("\"\n");
        write(processWrite[1], response.c_str(), strlen(response.c_str())+1);
        SysAdminRequestHandler::logger->info("Admin->Host:\n"+hostInput);
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

    // bzero(buffer, SocketUtils::MESSAGE_BYTES);
    // SocketUtils::receiveBuffer(adminSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    // if (DebugFlags::debugAuditingHub)
    //     cout << "Recieved: " << buffer << "\n";

    // string response(buffer);
    // vector<string> responseSplit = General::splitString(response);
    // if (responseSplit[0] == Messages::NOT_OK) {
    //     throw 10;  //TODO
    // }
}

void SysAdminRequestHandler::startSysAdminRequestHandler(SysAdminRequestHandler sysAdminRequestHandler, int adminToHubSocket) {
    std::cout << "SysAdminRequestHandler running with Auditing Hub with the name " << sysAdminRequestHandler.auditingHub->getHostname() << "\n";
    sysAdminRequestHandler.adminToHubSocket = adminToHubSocket;
    sysAdminRequestHandler.processAttestation(adminToHubSocket);
    //  std::this_thread::sleep_for(std::chrono::seconds(1));
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
       std::this_thread::sleep_for(std::chrono::seconds(1));

        string command(buffer);
       if(command.size() == Messages::OK_APPROVED.size()){
            bzero(buffer, SocketUtils::MESSAGE_BYTES);
            SocketUtils::receiveBuffer(adminToHubSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
            if (DebugFlags::debugAuditingHub)
                cout << "Recieved: " << buffer << "\n";
       }
        command = string(buffer);
        vector<string> commandSplit = General::splitString(command);
        bzero(buffer, SocketUtils::MESSAGE_BYTES);

        if (commandSplit[0].find(Messages::MANAGE) == string::npos) {
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