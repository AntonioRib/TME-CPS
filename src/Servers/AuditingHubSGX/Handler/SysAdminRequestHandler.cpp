#include "SysAdminRequestHandler.h"

const string UNCOMMITED_LOGS_DIR = "../../../Logs/Uncommited/";
const string COMMITED_LOGS_DIR = "../../../Logs/Commited/";

SysAdminRequestHandler::SysAdminRequestHandler() {
    std::cout << "SysAdminRequestHandler created\n";
}

SysAdminRequestHandler::SysAdminRequestHandler(AuditingHub* auditingHub) : auditingHub{auditingHub} {
    std::cout << "SysAdminRequestHandler created with the name " << auditingHub->getHostname() << "\n";
}

sgx_enclave_id_t sysAdminRequestHandler_eid = 0;

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
    pipe(processReadErr);
    pid_t pid = fork();
    if (pid == 0) {
        close(processWrite[1]);
        close(processRead[0]);
        close(processReadErr[0]);
        
        dup2(processWrite[0], fileno(stdin));
        dup2(processRead[1], fileno(stdout));
        dup2(processReadErr[1], fileno(stderr));

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

    string logPath(UNCOMMITED_LOGS_DIR + remoteHost + "." + adminUsername + "." + General::currentDateTime() + ".log");
    spdlog::drop_all();
    SysAdminRequestHandler::logger = spdlog::basic_logger_mt("Logger", logPath);
    spdlog::set_level(spdlog::level::trace); // Set global log level to everything
    spdlog::flush_on(spdlog::level::info);
    spdlog::set_default_logger(SysAdminRequestHandler::logger);
    SysAdminRequestHandler::logger->info("Logger started");
    cout << "Logger Started";
}

bool SysAdminRequestHandler::setNodeUntrusted(){
    struct hostent* serverHost;
    serverHost = SocketUtils::getHostByName(auditingHub->getMonitorHost());
    
    sockaddr_in monitorAddress;
    monitorAddress = SocketUtils::createServerAddress(Ports::MONITOR_AHUB_PORT+20);
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

bool SysAdminRequestHandler::exportMinion() {
    struct hostent* serverHost;
    serverHost = SocketUtils::getHostByName(SysAdminRequestHandler::remoteHost);

    sockaddr_in minionAddress;
    minionAddress = SocketUtils::createServerAddress(Ports::MINION_AHUB_PORT + 20);
    bcopy((char*)serverHost->h_addr, (char*)&minionAddress.sin_addr.s_addr, serverHost->h_length);
    // int serverSocket;
    // serverSocket = SocketUtils::createServerSocket(serverAddress);
    int minionSocket = socket(AF_INET, SOCK_STREAM, 0);
    SocketUtils::connectToServerSocket(minionSocket, minionAddress);
    if (DebugFlags::debugAuditingHub)
        cout << "Connected to the server\n";

    hostent* rHost = SocketUtils::getHostByName(this->remoteHost);
    char buffer[SocketUtils::MESSAGE_BYTES];
    std::string configuration = Messages::EXPORT + " " + inet_ntoa(minionAddress.sin_addr) + " " + Directories::APPS_DIR_IMPORTEXPORT + " " + "antoniorib";
    General::stringToCharArray(configuration, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(minionSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugAuditingHub)
        cout << "Wrote: " << buffer << " to Monitor\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(minionSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugAuditingHub)
        cout << "Recieved: " << buffer << "\n";

    string result(buffer);
    vector<string> resultSplit = General::splitString(result);

    if (resultSplit[0] == Messages::NOT_OK) {
        if (DebugFlags::debugAuditingHub)
            cout << "Unsucessfull purge\n";
        close(minionSocket);
        return false;
    } else if (resultSplit[0] == Messages::OK) {
        if (DebugFlags::debugAuditingHub)
            cout << "Sucessfull purge\n";
        close(minionSocket);
        return true;
    }
    if (DebugFlags::debugAuditingHub)
        cout << "Unknown command about purge\n";
    // close(minionSocket);
    return false;
}

bool SysAdminRequestHandler::purgeMinion(){
    struct hostent* serverHost;
    serverHost = SocketUtils::getHostByName(SysAdminRequestHandler::remoteHost);

    sockaddr_in monitorAddress;
    monitorAddress = SocketUtils::createServerAddress(Ports::MINION_AHUB_PORT+20);
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

bool SysAdminRequestHandler::importMinion() {
    struct hostent* serverHost;
    serverHost = SocketUtils::getHostByName(SysAdminRequestHandler::remoteHost);

    sockaddr_in minionAddress;
    minionAddress = SocketUtils::createServerAddress(Ports::MINION_AHUB_PORT + 20);
    bcopy((char*)serverHost->h_addr, (char*)&minionAddress.sin_addr.s_addr, serverHost->h_length);
    // int serverSocket;
    // serverSocket = SocketUtils::createServerSocket(serverAddress);
    int minionSocket = socket(AF_INET, SOCK_STREAM, 0);
    SocketUtils::connectToServerSocket(minionSocket, minionAddress);
    if (DebugFlags::debugAuditingHub)
        cout << "Connected to the server\n";

    hostent* rHost = SocketUtils::getHostByName(this->remoteHost);

    char buffer[SocketUtils::MESSAGE_BYTES];
    std::string configuration = Messages::IMPORT + " " + Directories::APPS_DIR_IMPORTEXPORT;
    General::stringToCharArray(configuration, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(minionSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugAuditingHub)
        cout << "Wrote: " << buffer << " to Monitor\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(minionSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugAuditingHub)
        cout << "Recieved: " << buffer << "\n";

    string result(buffer);
    vector<string> resultSplit = General::splitString(result);

    if (resultSplit[0] == Messages::NOT_OK) {
        if (DebugFlags::debugAuditingHub)
            cout << "Unsucessfull purge\n";
        close(minionSocket);
        return false;
    } else if (resultSplit[0] == Messages::OK) {
        if (DebugFlags::debugAuditingHub)
            cout << "Sucessfull purge\n";
        close(minionSocket);
        return true;
    }
    if (DebugFlags::debugAuditingHub)
        cout << "Unknown command about purge\n";
    // close(minionSocket);
    return false;
}

bool SysAdminRequestHandler::launchManagementSession(){
    if (DebugFlags::debugAuditingHub)
        cout << "Will set and purge stuff.\n";
    
    auto start = chrono::high_resolution_clock::now(); 

    if(!setNodeUntrusted())
        return false;

    if(!purgeMinion())
        return false;
    
    launchSessionProcess();
    launchLogger();
    
    auto stop = chrono::high_resolution_clock::now(); 
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start); 

    cout << "Time taken to start session: " << duration.count() << " milliseconds" << endl;

    if (DebugFlags::debugAuditingHub)
        cout << "Session created. Sending prompt to admin.\n";
        
    std::this_thread::sleep_for(std::chrono::seconds(1));
    string promptString = "[" + adminUsername + "@" + remoteHost + "]> ";
    string promptStringCmd = string("echo ") + string("\"\n") + promptString+ string("\"") + "\n";
    write(processWrite[1], promptStringCmd.c_str(), strlen(promptStringCmd.c_str()));
    if (DebugFlags::debugAuditingHub)
        cout << "Wrote: " << promptStringCmd << " to Pipe\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));

    string hostInput;
    string hostOutput;
    std::ostringstream oss;
    char buffer[SocketUtils::MESSAGE_BYTES];
    while(true){
        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        int nbytes = read(processRead[0], buffer, sizeof(buffer));
        if(nbytes <= 0)
            nbytes = read(processReadErr[0], buffer, sizeof(buffer));
        hostOutput = string(buffer); //READ from process
        hostOutput.pop_back();
        oss << hostOutput;

        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        General::stringToCharArray(hostOutput, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(adminToHubSocket, buffer, strlen(buffer), 0);
        cout << "hostOutput: " << hostOutput << "\n";
        cout << "promptString: " << promptString << "\n";
        if (hostOutput.find(promptString) == std::string::npos)
            continue;

        if (DebugFlags::debugAuditingHub)
            cout << "Going to log: " << "Host->Admin:\n"+oss.str() << "\n";
        SysAdminRequestHandler::logger->info("\nHost->Admin:\n"+oss.str());
        oss.str(string());

        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::receiveBuffer(adminToHubSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
        hostInput = string(buffer);
        if (DebugFlags::debugAuditingHub)
            cout << "Recieved: " << buffer << "\n";
        if (hostInput == Messages::QUIT){
            if (DebugFlags::debugAuditingHub)
                cout << "Session has ended\n";
            //auditingHubToNodeSessionProcess destroy process
            auditingHub->removeSession(remoteHost);
            return true;
        }

        string response = hostInput + string("; echo ") + string("\"\n") + promptString + string("\"") + "\n";
        write(processWrite[1], response.c_str(), strlen(response.c_str()));
         if (DebugFlags::debugAuditingHub)
            cout << "Going to log: " << "Admin->Host:\n"+hostInput << "\n";
        SysAdminRequestHandler::logger->info("\nAdmin->Host:\n"+hostInput);
    }
}

bool SysAdminRequestHandler::launchUrgentManagementSession() {
    if (DebugFlags::debugAuditingHub)
        cout << "Will set and purge stuff.\n";

    auto start = chrono::high_resolution_clock::now();

    if (!setNodeUntrusted())
        return false;

    if (!exportMinion())
        return false;

    if (!importMinion())
        return false;

    if (!purgeMinion())
        return false;

    launchSessionProcess();
    launchLogger();

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

    cout << "Time taken to start session: " << duration.count() << " milliseconds" << endl;

    if (DebugFlags::debugAuditingHub)
        cout << "Session created. Sending prompt to admin.\n";

    std::this_thread::sleep_for(std::chrono::seconds(1));
    string promptString = "[" + adminUsername + "@" + remoteHost + "]> ";
    string promptStringCmd = string("echo ") + string("\"\n") + promptString + string("\"") + "\n";
    write(processWrite[1], promptStringCmd.c_str(), strlen(promptStringCmd.c_str()));
    if (DebugFlags::debugAuditingHub)
        cout << "Wrote: " << promptStringCmd << " to Pipe\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));

    string hostInput;
    string hostOutput;
    std::ostringstream oss;
    char buffer[SocketUtils::MESSAGE_BYTES];
    while (true) {
        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        int nbytes = read(processRead[0], buffer, sizeof(buffer));
        if (nbytes <= 0)
            nbytes = read(processReadErr[0], buffer, sizeof(buffer));
        hostOutput = string(buffer);  //READ from process
        hostOutput.pop_back();
        oss << hostOutput;

        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        General::stringToCharArray(hostOutput, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(adminToHubSocket, buffer, strlen(buffer), 0);
        cout << "hostOutput: " << hostOutput << "\n";
        cout << "promptString: " << promptString << "\n";
        if (hostOutput.find(promptString) == std::string::npos)
            continue;

        if (DebugFlags::debugAuditingHub)
            cout << "Going to log: "
                 << "Host->Admin:\n" + oss.str() << "\n";
        SysAdminRequestHandler::logger->info("\nHost->Admin:\n" + oss.str());
        oss.str(string());

        bzero(buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::receiveBuffer(adminToHubSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
        hostInput = string(buffer);
        if (DebugFlags::debugAuditingHub)
            cout << "Recieved: " << buffer << "\n";
        if (hostInput == Messages::QUIT) {
            if (DebugFlags::debugAuditingHub)
                cout << "Session has ended\n";
            //auditingHubToNodeSessionProcess destroy process
            auditingHub->removeSession(remoteHost);
            return true;
        }

        string response = hostInput + string("; echo ") + string("\"") + promptString + string("\"") + "\n";
        write(processWrite[1], response.c_str(), strlen(response.c_str()));
        if (DebugFlags::debugAuditingHub)
            cout << "Going to log: "
                 << "Admin->Host:\n" + hostInput << "\n";
        SysAdminRequestHandler::logger->info("\nAdmin->Host:\n" + hostInput);
    }
}

void SysAdminRequestHandler::processAttestation(int adminSocket) {

    if (initialize_enclave(&sysAdminRequestHandler_eid, "SysAdminRequestHandler.token", "enclave.signed.so") < 0) {
        std::cout << "Fail to initialize enclave." << std::endl;
        return;
    }
    
    int result;
    sgx_status_t status = sysAdminTrustedProcessAttestation(sysAdminRequestHandler_eid, &result, adminSocket, (char*)auditingHub->getApprovedSHA1().c_str(), 
                                                            (char*)auditingHub->getApprovedSHA1().c_str(), SocketUtils::MESSAGE_BYTES);
    if (status != SGX_SUCCESS) {
        std::cout << "Enclave call Failed" << std::endl;
        return;
    }
    std::cout << "Enclave call Success" << std::endl;

    if(result == 0)
        throw runtime_error("SysAdmin rejected platform attestation\n"); 
    if (DebugFlags::debugMonitor)
        cout << "SysAdmin accepted platform attestation\n";
    return;

    //**//
    // char buffer[SocketUtils::MESSAGE_BYTES];
    // bzero(buffer, SocketUtils::MESSAGE_BYTES);
    // SocketUtils::receiveBuffer(adminSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    // if (DebugFlags::debugAuditingHub)
    //     cout << "Recieved: " << buffer << "\n";

    // string request(buffer);
    // vector<string> requestSplit = General::splitString(request);
    // if (requestSplit[0] == Messages::ATTEST) {
    //     if (DebugFlags::debugAuditingHub)
    //         cout << "Being attested\n";

    //     bzero(buffer, SocketUtils::MESSAGE_BYTES);
    //     std::string quote = string(Messages::QUOTE) + " " + string(AttestationConstants::QUOTE) + " " + string(auditingHub->getApprovedSHA1()) + " " +  string(auditingHub->getApprovedSHA1());
    //     General::stringToCharArray(quote, buffer, SocketUtils::MESSAGE_BYTES);
    //     SocketUtils::sendBuffer(adminSocket, buffer, strlen(buffer), 0);
    //     if (DebugFlags::debugAuditingHub)
    //         cout << "Wrote: " << buffer << " to client\n";
    // } else 
    //     throw runtime_error("Recieved something unknown"); //TODO

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
        cout << "Waiting for messages...";
        // clientSocket = SocketUtils::acceptClientSocket(serverSocket);
        // cout << "Got connection from client\n";
        try {
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

            bool urgentSession = commandSplit[0].find(Messages::URGENTMANAGE) != string::npos;
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

            if (urgentSession)
                launchManagementSessionResult = sysAdminRequestHandler.launchUrgentManagementSession();
            else
                launchManagementSessionResult = sysAdminRequestHandler.launchManagementSession();

            if (launchManagementSessionResult){
                std::string response = Messages::OK;
                bzero(buffer, SocketUtils::MESSAGE_BYTES);
                General::stringToCharArray(response, buffer, SocketUtils::MESSAGE_BYTES);
                SocketUtils::sendBuffer(adminToHubSocket, buffer, strlen(buffer), 0);
                if (DebugFlags::debugAuditingHub)
                    cout << "Wrote: " << buffer << " to client\n";
                break;
            } else {
                std::string response = Messages::NOT_OK;
                bzero(buffer, SocketUtils::MESSAGE_BYTES);
                General::stringToCharArray(response, buffer, SocketUtils::MESSAGE_BYTES);
                SocketUtils::sendBuffer(adminToHubSocket, buffer, strlen(buffer), 0);
                if (DebugFlags::debugAuditingHub)
                    cout << "Wrote: " << buffer << " to client\n";
                break;
            }
            close(adminToHubSocket);
        } catch (const exception& e){
            cout << e.what() << '\n';
        }
    }
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create SysAdminRequestHandler\n";

//     SysAdminRequestHandler* sysAdminRequestHandler;
//     sysAdminRequestHandler = new SysAdminRequestHandler();

//     std::cout << "Bubye\n";

// }