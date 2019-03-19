#include "DeveloperRequestHandler.h"
using namespace std;

const int ERROR_CODE = 255;

// const int MESSAGE_BYTES = 2048;

DeveloperRequestHandler::DeveloperRequestHandler() {
    std::cout << "DeveloperRequestHandler created\n";
}

DeveloperRequestHandler::DeveloperRequestHandler(Monitor* monitor) : monitor{monitor} {
    std::cout << "DeveloperRequestHandler created with the name " << monitor->getHostname() << "\n";
}

sgx_enclave_id_t developerRequestHandler_eid = 0;

bool DeveloperRequestHandler::sendApp(Minion* minion, string appDir){
    char* scpArgsStream;
    int size = asprintf(&scpArgsStream, "%s -r -i %s -oStrictHostKeyChecking=no /home/AntonioRib/%s%s %s@%s:%s%s",
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
        if (result == -1) {
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

bool DeveloperRequestHandler::deployApp(string appID, int instances){
    vector<Minion*> trustedMinions = monitor->pickNTrustedMinions(instances);
    monitor->addApplication(appID, trustedMinions);

    bool copyResult = true;
    for_each(trustedMinions.begin(), trustedMinions.end(),
             [&](Minion* minion) {
                 if(copyResult){
                    if(DebugFlags::debugMonitor)
                        cout << "Uploading app to minion";
                    copyResult &= sendApp(minion, appID);
                    if (!copyResult)
                        copyResult = false;

                    struct hostent* minionHost;
                    minionHost = SocketUtils::getHostByName(minion->getIpAddress());

                    sockaddr_in minionAddress;
                    minionAddress = SocketUtils::createServerAddress(Ports::MINION_MONITOR_PORT);
                    // minionAddress.sin_addr.s_addr = inet_addr(minion->getIpAddress().c_str());  // minion.getIpAddress();
                    bcopy((char *)minionHost->h_addr, (char *)&minionAddress.sin_addr.s_addr, minionHost->h_length);
                    int minionSocket = socket(AF_INET, SOCK_STREAM, 0);
                    SocketUtils::connectToServerSocket(minionSocket, minionAddress);

                    char buffer[SocketUtils::MESSAGE_BYTES];
                    string message = Messages::DEPLOY + " " + appID;
                    General::stringToCharArray(message, buffer, SocketUtils::MESSAGE_BYTES);
                    SocketUtils::sendBuffer(minionSocket, buffer, strlen(buffer), 0);
                    if (DebugFlags::debugMonitor)
                        cout << "Wrote: " << buffer << " to Minion\n";

                    bzero(buffer, SocketUtils::MESSAGE_BYTES);
                    SocketUtils::receiveBuffer(minionSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
                    if (DebugFlags::debugMonitor)
                        cout << "Recieved: " << buffer << " from Minion\n";

                    string resultMessage(buffer);
                    vector<string> resultSplit = General::splitString(resultMessage);

                    if (resultSplit[0] == Messages::OK){
                        // continue;
                    } else if (resultSplit[0] == Messages::NOT_APPROVED) {
                        copyResult = false;
                    }
                 }
             });
    return copyResult;
}

bool DeveloperRequestHandler::deleteApp(string appID){
    vector<Minion*> appHosts = monitor->getHosts(appID);
    monitor->deleteApplication(appID);

    bool result = true;
    for_each(appHosts.begin(), appHosts.end(),
             [&](Minion* minion) {
                 if (result) {
                     if (DebugFlags::debugMonitor)
                         cout << "Deleting app to minion";

                    struct hostent* minionHost;
                    minionHost = SocketUtils::getHostByName(minion->getIpAddress());

                     sockaddr_in minionAddress;
                     minionAddress = SocketUtils::createServerAddress(Ports::MINION_MONITOR_PORT);
                    //  minionAddress.sin_addr.s_addr = inet_addr(minion->getIpAddress().c_str());  // minion.getIpAddress();
                    bcopy((char *)minionHost->h_addr, (char *)&minionAddress.sin_addr.s_addr, minionHost->h_length);
                     int minionSocket = socket(AF_INET, SOCK_STREAM, 0);
                     SocketUtils::connectToServerSocket(minionSocket, minionAddress);

                     char buffer[SocketUtils::MESSAGE_BYTES];
                     bzero(buffer, SocketUtils::MESSAGE_BYTES);
                     string message = Messages::DELETE_APP + " " + appID;
                     General::stringToCharArray(message, buffer, SocketUtils::MESSAGE_BYTES);
                     SocketUtils::sendBuffer(minionSocket, buffer, strlen(buffer), 0);
                     if (DebugFlags::debugMonitor)
                         cout << "Wrote: " << buffer << " to Minion\n";

                     bzero(buffer, SocketUtils::MESSAGE_BYTES);
                     SocketUtils::receiveBuffer(minionSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
                     if (DebugFlags::debugMonitor)
                         cout << "Recieved: " << buffer << " from Minion\n";

                     string resultMessage(buffer);
                     vector<string> resultSplit = General::splitString(resultMessage);

                     if (resultSplit[0] == Messages::OK) {
                         //  continue;
                     } else if (resultSplit[0] == Messages::NOT_OK) {
                         result = false;
                     }
                 }
             });
    return result;
}

void DeveloperRequestHandler::processAttestation(int developerSocket) {

    if (initialize_enclave(&developerRequestHandler_eid, "DeveloperRequestHandler.token", "enclave.signed.so") < 0) {
        std::cout << "Fail to initialize enclave." << std::endl;
        return;
    }
    
    int result;// = (char*)malloc(sizeof(char)*SocketUtils::MESSAGE_BYTES);
    sgx_status_t status = developerTrustedProcessAttestation(developerRequestHandler_eid, &result, developerSocket, (char*)monitor->getApprovedSHA1().c_str(), 
                                                            (char*)monitor->getApprovedSHA1().c_str(), SocketUtils::MESSAGE_BYTES);
    if (status != SGX_SUCCESS) {
        std::cout << "Enclave call Failed" << std::endl;
        return;
    }
    std::cout << "Enclave call Success" << std::endl;
    if(result == 0)
        throw runtime_error("Developer rejected platform attestation\n"); 
    if (DebugFlags::debugMonitor)
        cout << "Developer accepted platform attestation\n";
    return;
}

void DeveloperRequestHandler::startDeveloperRequestHandler(DeveloperRequestHandler developerRequestHandler) {
    std::cout << "DeveloperRequestHandler running with Monitor with the name " << developerRequestHandler.monitor->getHostname() << "\n";

    sockaddr_in developerAddress;
    developerAddress = SocketUtils::createServerAddress(Ports::MONITOR_DEVELOPER_PORT);

    int serverSocket;
    serverSocket = SocketUtils::createServerSocket(developerAddress);

    int developerSocket;
    while (true) {
        cout << "Waiting for connections...\n";
        try{
            developerSocket = SocketUtils::acceptClientSocket(serverSocket);
            cout << "Got connection from Developer\n";

            developerRequestHandler.processAttestation(developerSocket);
            char buffer[SocketUtils::MESSAGE_BYTES];
            bzero(buffer, SocketUtils::MESSAGE_BYTES);
            SocketUtils::receiveBuffer(developerSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
            if (DebugFlags::debugMonitor)
                cout << "Recieved: " << buffer << "\n";
            // cout << buffer;
            string command(buffer);
            vector<string> commandSplit = General::splitString(command);

            bool requestResult = false;
            if (commandSplit[0] == Messages::NEW_APP) {
                if (DebugFlags::debugMonitor)
                    cout << "Deploying: " << commandSplit[2] << "\n";
                requestResult = developerRequestHandler.deployApp(commandSplit[2], atoi(commandSplit[3].c_str()));
            } else if (commandSplit[0] == Messages::DELETE_APP) {
                if (DebugFlags::debugMonitor)
                    cout << "Deleting: " << commandSplit[2] << "\n";
                requestResult = developerRequestHandler.deleteApp(commandSplit[2]);
            }

            if (requestResult){
                bzero(buffer, SocketUtils::MESSAGE_BYTES);
                std::string result = Messages::OK;
                General::stringToCharArray(result, buffer, SocketUtils::MESSAGE_BYTES);
                SocketUtils::sendBuffer(developerSocket, buffer, strlen(buffer), 0);
                if (DebugFlags::debugMonitor)
                    cout << "Success \n";
            } else {
                bzero(buffer, SocketUtils::MESSAGE_BYTES);
                std::string result = Messages::NOT_OK;
                General::stringToCharArray(result, buffer, SocketUtils::MESSAGE_BYTES);
                SocketUtils::sendBuffer(developerSocket, buffer, strlen(buffer), 0);
                if (DebugFlags::debugMonitor)
                    cout << "Failed \n";
            }
        } catch (const exception& e){
            close(developerSocket);
            cout << e.what() << '\n';
        }
    }

    close(developerSocket);
    close(serverSocket);
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create DeveloperRequestHandler\n";

//     DeveloperRequestHandler* developerRequestHandler;
//     developerRequestHandler = new DeveloperRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }