#include "AHubRequestHandler.h"
using namespace std;

const int ERROR_CODE = 255;
// const int MESSAGE_BYTES = 2048;
sgx_enclave_id_t aHubRequestHandler_eid = 0;

AHubRequestHandler::AHubRequestHandler() {
    std::cout << "AHubRequestHandler created\n";
}

bool AHubRequestHandler::sendApp(Minion* minion, std::string appDir){
    char* scpArgsStream;
    int size = asprintf(&scpArgsStream, "%s -r -i %s -oStrictHostKeyChecking=no ./%s%s %s@%s:%s%s",
                        ProcessBinaries::SCP.c_str(), sshKey.c_str(), Directories::APPS_DIR_MONITOR.c_str(), appDir.c_str(),
                        username.c_str(), minion->getIpAddress().c_str(), Directories::APPS_DIR_MINION.c_str(), appDir.c_str());
    string scpArgsStreamStr(scpArgsStream);
    std::vector<std::string> scpArgsStreamVec = General::splitString(scpArgsStreamStr);
    char* scpArgsStreamCharVec[scpArgsStreamVec.size()];
    int i = 0;
    for (const std::string& str : scpArgsStreamVec) {
        scpArgsStreamCharVec[i] = const_cast<char*>(str.c_str());
        i++;
    }
    scpArgsStreamCharVec[i] = NULL;

    if (DebugFlags::debugMonitor)
        cout << "Executing command: " << scpArgsStream << "\n";
    fflush(NULL);
    pid_t pid = fork();
    if (pid == 0) {
        int result = execvp(ProcessBinaries::SCP.c_str(), scpArgsStreamCharVec);
        if (result == -1){
            if (DebugFlags::debugMonitor){
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
        if (WIFEXITED(status) && WEXITSTATUS(status) == ERROR_CODE) 
            return false;
    } 
    return true;
}

bool AHubRequestHandler::deployAppOnMinion(Minion* host, string appId) {
    bool scpResult;
    string deployResult;

    if (DebugFlags::debugMonitor)
        cout << "Uploading app to minion\n";
    scpResult &= sendApp(host, appId);

    struct hostent* minionHost;
    minionHost = SocketUtils::getHostByName(host->getIpAddress());

    sockaddr_in minionAddress;
    minionAddress = SocketUtils::createServerAddress(Ports::MINION_MONITOR_PORT+20);
    bcopy((char *)minionHost->h_addr, (char *)&minionAddress.sin_addr.s_addr, minionHost->h_length);
    int minionSocket = socket(AF_INET, SOCK_STREAM, 0);

    SocketUtils::connectToServerSocket(minionSocket, minionAddress);
    if (DebugFlags::debugMonitor)
        cout << "Got connection to Minion\n";

    char buffer[SocketUtils::MESSAGE_BYTES];
    std::string request = Messages::DEPLOY + " " + appId;
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
    if (responseSplit[0] == Messages::OK) {
        return scpResult;
    } else {
        return false;
    }
}

string AHubRequestHandler::spawnReplacementInstances(Minion* untrustedMinion) {
    map<string, Minion*> trustedMinions = monitor->getTrustedMinions();
    if (trustedMinions.size() == 0)
        return "NULL";
    vector<string> trustedSet;
    for (map<string, Minion*>::iterator it = trustedMinions.begin(); it != trustedMinions.end(); ++it) {
        trustedSet.push_back(it->first);
    }
    bool spawnResult = true;
    map<string, Application*> untrustedMinionApplications = untrustedMinion->getApplications();
    string minionHandler = trustedSet.front();
    Minion* minion = trustedMinions[minionHandler];
    for (const auto& entry : untrustedMinionApplications) {
        untrustedMinion->removeApp(entry.first);
        minion->addApp(entry.second);
    }
    return minionHandler;
}

bool AHubRequestHandler::attestMinion(std::string untrustedMinion) {
    struct hostent* minionHost;
    minionHost = SocketUtils::getHostByName(untrustedMinion);
    sockaddr_in minionAddress;
    minionAddress = SocketUtils::createServerAddress(Ports::MINION_MONITOR_PORT+20);
    bcopy((char *)minionHost->h_addr, (char *)&minionAddress.sin_addr.s_addr, minionHost->h_length);
    int minionSocket = socket(AF_INET, SOCK_STREAM, 0);

    SocketUtils::connectToServerSocket(minionSocket, minionAddress);
    cout << "Got connection to Minion\n";

    while (monitor->getApprovedConfiguration() == (unsigned char*)General::NULL_VALUE) {
        std::cout << "Waiting for approved config. Current Config -> " << +monitor->getApprovedConfiguration() << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (initialize_enclave(&aHubRequestHandler_eid, "AHubRequestHandler.token", "enclave.signed.so") < 0) {
        std::cout << "Fail to initialize enclave." << std::endl;
        return false;
    }
    
    int result;
    sgx_status_t status = trustedAttestMinionReturn(aHubRequestHandler_eid, &result, minionSocket, SocketUtils::MESSAGE_BYTES);
    if (status != SGX_SUCCESS) {
        std::cout << "Failed" << std::endl;
    }
    std::cout << "Success" << std::endl;

    if(result == 1)
        return true;
    return false;
}

void AHubRequestHandler::setMinionUntrustedOnMonitor(string untrustedMinion) {
    if (DebugFlags::debugMonitor)
        cout << "Setting Minion with IP " + untrustedMinion + " untrusted\n";
    monitor->setMinionUntrusted(untrustedMinion);
}

void AHubRequestHandler::setMinionTrustedOnMonitor(string trustedMinion) {
    if (DebugFlags::debugMonitor)
        cout << "Setting Minion with IP " + trustedMinion + " trusted\n";
    monitor->setMinionTrusted(trustedMinion);
}

AHubRequestHandler::AHubRequestHandler(Monitor* monitor) : monitor{monitor} {
    std::cout << "AHubRequestHandler created with the name " << monitor->getHostname() << "\n";
}

void AHubRequestHandler::startAHubRequestHandler(AHubRequestHandler aHubRequestHandler) {
    std::cout << "AHubRequestHandler running with Monitor with the name " << aHubRequestHandler.monitor->getHostname() << "\n";

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::MONITOR_AHUB_PORT);

    int serverSocket;
    serverSocket = SocketUtils::createServerSocket(serverAddress);

    int hubSocket;
    while (true) {
        cout << "Waiting for connections...\n";
        try{
            
            hubSocket = SocketUtils::acceptClientSocket(serverSocket);
            cout << "Got connection from AuditingHub\n";

            char buffer[SocketUtils::MESSAGE_BYTES];
            bzero(buffer, SocketUtils::MESSAGE_BYTES);
            SocketUtils::receiveBuffer(hubSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
            if (DebugFlags::debugMonitor)
                cout << "Recieved: " << buffer << "\n";

            string command(buffer);
            vector<string> commandSplit = General::splitString(command);

            bool requestResult = false;
            string newMinion = "NULL";
            if (commandSplit[0] == Messages::SET_TRUSTED) {
                requestResult = aHubRequestHandler.attestMinion(commandSplit[1]);
                aHubRequestHandler.setMinionTrustedOnMonitor(commandSplit[1]);
            } else if (commandSplit[0] == Messages::SET_UNTRUSTED) {
                aHubRequestHandler.setMinionUntrustedOnMonitor(commandSplit[1]);
                requestResult = true;
            } else if (commandSplit[0] == Messages::SET_UNTRUSTEDURGENT) {
                aHubRequestHandler.setMinionUntrustedOnMonitor(commandSplit[1]);
                Minion* untrustedMinion = aHubRequestHandler.monitor->getUntrustedMinion(commandSplit[1]);
                newMinion = aHubRequestHandler.spawnReplacementInstances(untrustedMinion);
                requestResult = true;
            }

            if (requestResult) {
                bzero(buffer, SocketUtils::MESSAGE_BYTES);
                std::string result = Messages::OK;
                if (newMinion != "NULL")
                    result = Messages::OK + " " + newMinion;
                else
                    result = Messages::OK + " " + commandSplit[1];
                General::stringToCharArray(result, buffer, SocketUtils::MESSAGE_BYTES);
                SocketUtils::sendBuffer(hubSocket, buffer, strlen(buffer), 0);
                if (DebugFlags::debugMonitor)
                    cout << "Success \n";
            } else {
                bzero(buffer, SocketUtils::MESSAGE_BYTES);
                std::string result = Messages::NOT_OK + " NULL";
                General::stringToCharArray(result, buffer, SocketUtils::MESSAGE_BYTES);
                SocketUtils::sendBuffer(hubSocket, buffer, strlen(buffer), 0);
                if (DebugFlags::debugMonitor)
                    cout << "Failed \n";
            }
        } catch (const exception& e){
            close(hubSocket);
            cout << e.what() << '\n';
        }
    }

    close(hubSocket);
    close(serverSocket);
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create AHubRequestHandler\n";

//     AHubRequestHandler* ahubRequestHandler;
//     ahubRequestHandler = new AHubRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }