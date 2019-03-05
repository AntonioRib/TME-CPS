#include "MinionMonitorRequestHandler.h"
using namespace std;

const int ERROR_CODE = 255;

MinionMonitorRequestHandler::MinionMonitorRequestHandler() {
    std::cout << "MonitorRequestHandler created\n";
}

MinionMonitorRequestHandler::MinionMonitorRequestHandler(Minion* minion) : minion{minion} {
    std::cout << "MonitorRequestHandler created with the name " << minion->getIpAddress() << "\n";
}

sgx_enclave_id_t minionMonitorRequestHandler_eid = 0;

bool MinionMonitorRequestHandler::deployApp(string appId) {
    // string createString = "sudo docker build -t %s-container /home/AntonioRib/%s%s"; 
    // string deployString = "sudo docker run -p 80 -d --name %s %s-container";
    string createString = "sudo docker pull redis";
    string deployString = "sudo docker run -d -p 6379:6379 -i -t redis";

    char* createContainerArgsStream;
    int sizeCreateStream = asprintf(&createContainerArgsStream, "sudo docker pull redis", appId.c_str(), Directories::APPS_DIR_MINION.c_str(), appId.c_str());
    string createContainerArgsStreamStr(createContainerArgsStream);
    std::vector<std::string> createContainerArgsStreamVec = General::splitString(createContainerArgsStreamStr);
    char* createContainerArgsStreamCharVec[createContainerArgsStreamVec.size()];
    int i = 0;
    for (const std::string& str : createContainerArgsStreamVec) {
        createContainerArgsStreamCharVec[i] = const_cast<char*>(str.c_str());
        i++;
    }
    createContainerArgsStreamCharVec[i] = NULL;

    char* deployContainerArgsStream;
    int sizeDeployStream = asprintf(&deployContainerArgsStream, "sudo docker run -d -p 6379:6379 -i -t redis", appId.c_str(), appId.c_str());
    string deployContainerArgsStreamStr(deployContainerArgsStream);
    std::vector<std::string> deployContainerArgsStreamVec = General::splitString(deployContainerArgsStreamStr);
    char* deployContainerArgsStreamCharVec[deployContainerArgsStreamVec.size()];
    i = 0;
    for (const std::string& str : deployContainerArgsStreamVec) {
        deployContainerArgsStreamCharVec[i] = const_cast<char*>(str.c_str());
        i++;
    }
    deployContainerArgsStreamCharVec[i] = NULL;

    if (DebugFlags::debugMonitor)
        cout << "Executing command: " << createContainerArgsStream << "\n";
    fflush(NULL);
    pid_t pidCreate = fork();
    if (pidCreate == 0) {
        int result = execvp(createContainerArgsStreamCharVec[0], createContainerArgsStreamCharVec);
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
        if (WIFEXITED(status) && WEXITSTATUS(status) == ERROR_CODE) 
            return false;
    }

    if (DebugFlags::debugMonitor)
        cout << "Executing command: " << deployContainerArgsStream << "\n";
    fflush(NULL);
    pid_t pidDeploy = fork();
    if (pidDeploy == 0) {
        int result = execvp(deployContainerArgsStreamCharVec[0], deployContainerArgsStreamCharVec);
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
        if (WIFEXITED(status) && WEXITSTATUS(status) == ERROR_CODE) 
            return false;
    }

    return true;
}

bool MinionMonitorRequestHandler::deleteApp(string appId) {
    char* deleteArgsStream;
    int size = asprintf(&deleteArgsStream, "sudo ./%s", Scripts::DELETE_APP.c_str());
    string deleteArgsStreamStr(deleteArgsStream);
    std::vector<std::string> deleteArgsStreamVec = General::splitString(deleteArgsStreamStr);
    char* deleteArgsStreamCharVec[deleteArgsStreamVec.size()];
    int i = 0;
    for (const std::string& str : deleteArgsStreamVec) {
        deleteArgsStreamCharVec[i] = const_cast<char*>(str.c_str());
        i++;
    }
    deleteArgsStreamCharVec[i] = NULL;

    if (DebugFlags::debugMonitor)
        cout << "Executing command: " << deleteArgsStream << "\n";
    fflush(NULL);
    pid_t pid = fork();
    if (pid == 0) {
        int result = execvp(deleteArgsStreamCharVec[0], deleteArgsStreamCharVec);
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

bool MinionMonitorRequestHandler::processAttestation(int monitorSocket, string nonce) {
    
    if (initialize_enclave(&minionMonitorRequestHandler_eid, "MinionMonitorRequestHandler.token", "enclave.signed.so") < 0) {
        std::cout << "Fail to initialize enclave." << std::endl;
        return false;
    }
    
    int result;// = (char*)malloc(sizeof(char)*SocketUtils::MESSAGE_BYTES);
    sgx_status_t status = minionMonitorRequestTrustedProcessAttestation(minionMonitorRequestHandler_eid, &result, monitorSocket, SocketUtils::MESSAGE_BYTES);
    if (status != SGX_SUCCESS) {
        std::cout << "Enclave call Failed" << std::endl;
        return false;
    }
    std::cout << "Enclave call Success" << std::endl;
    if(result == 0){
        cout << "Not approved!\n";
        return false;
    } 
    
    if (DebugFlags::debugMonitor){
        cout << "Approved!\n";
    }
    return true;
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
        cout << "Waiting for connections...\n";
        try {
         
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
                requestResult = minionMonitorRequestHandler->processAttestation(monitorSocket, commandSplit[1]);
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
        } catch (const exception& e){
            close(monitorSocket);
            cout << e.what() << '\n';
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