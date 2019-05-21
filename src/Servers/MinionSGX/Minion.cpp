#include "Minion.h"
#include "Handler/MinionAHubRequestHandler.h"
#include "Handler/MinionMonitorRequestHandler.h"
#include "../../Utilities/TPM.h"
#include <iterator>
using namespace std;

// const int MESSAGE_BYTES = 2048;

const int MONITOR_HOST_FLAG_INDEX = 1;
const int HOSTNAME_FLAG_INDEX = 3;
const int IPADDRESS_FLAG_INDEX = 5;

sgx_enclave_id_t minion_eid = 0;

// OCall implementations
void ocall_print(const char* str) {
    printf("%s\n", str);
}

void ocall_socketSendBuffer(int minionSocket, char* buffer){
    if (DebugFlags::debugMinion)
        cout << "OCAL: Will write: " << buffer << " to minion\n";
    SocketUtils::sendBuffer(minionSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugMinion)
        cout << "OCAL: Wrote: " << buffer << " to minion\n";
}

void ocall_socketReceiveBuffer(int minionSocket, char* buffer, size_t bufferLength){
    SocketUtils::receiveBuffer(minionSocket, buffer, bufferLength - 1, 0);
        if (DebugFlags::debugMinion)
        cout << "OCAL: Recieved from minion: " << buffer << "\n";
}

void ocall_socketReadTPM(char* tpmOut, size_t tpmOutLength){
    TPM2B_ATTEST *quoted = NULL;
	TPMT_SIGNATURE *signature = NULL;
    TPM::tpm_quote(quoted, signature);
    	// cout << "quoted: " << quoted << "\n";
        // cout << "signature: " << quoted << "\n";
    std::this_thread::sleep_for (std::chrono::seconds(2));
}

Minion::Minion()
{
    std::cout << "Minion created\n";
}

Minion::Minion(const Minion &m) : monitorHost{m.monitorHost}, hostname{m.hostname} {
    // std::cout << "Minion created with the name " << Minion::name << " - copy constructor \n";
    Minion::startMonitorHandler();
    Minion::startAuditingHubHandler();
}

Minion::Minion(string monitorHost, string hostname, string ipAddress) : monitorHost{monitorHost}, hostname{hostname}, ipAddress{ipAddress} {
    std::cout << "Minion created with the name " << Minion::hostname << " - " << Minion::ipAddress << " string constructor \n";
    bool handlersStartResult = true;
    handlersStartResult &= Minion::startMonitorHandler();
    handlersStartResult &= Minion::startAuditingHubHandler();

    if (handlersStartResult){
        while (true) {
            this_thread::yield();
        }
    }
}

string Minion::getMonitorHost(){
    return Minion::monitorHost;
}
string Minion::getHostname(){
    return Minion::hostname;
}

string Minion::getIpAddress(){
    return Minion::ipAddress;
}

void Minion::processAttestation(int monitorSocket){
        
    if (initialize_enclave(&minion_eid, "Minion.token", "enclave.signed.so") < 0) {
        std::cout << "Fail to initialize enclave." << std::endl;
        return;
    }
    
    int result;// = (char*)malloc(sizeof(char)*SocketUtils::MESSAGE_BYTES);
    sgx_status_t status = minionTrustedProcessAttestation(minion_eid, &result, monitorSocket, SocketUtils::MESSAGE_BYTES);
    if (status != SGX_SUCCESS) {
        std::cout << "Enclave call Failed" << std::endl;
        return;
    }
    std::cout << "Enclave call Success" << std::endl;
    if(result == 0){
        cout << "Not approved!\n";
        throw runtime_error("Monitor rejected platform attestation\n"); 
    }

    return;
}

bool Minion::startMonitorHandler(){
    struct hostent* serverHost;
    serverHost = SocketUtils::getHostByName(Minion::monitorHost);
    cout << "Server host address: " << (char *)serverHost->h_addr << "\n";

    sockaddr_in monitorAddress;
    monitorAddress = SocketUtils::createServerAddress(Ports::MONITOR_MINION_PORT+20);
    bcopy((char *)serverHost->h_addr, (char *)&monitorAddress.sin_addr.s_addr, serverHost->h_length);
    
    int monitorSocket = socket(AF_INET, SOCK_STREAM, 0);
    SocketUtils::connectToServerSocket(monitorSocket, monitorAddress);
    if (DebugFlags::debugAuditor)
        cout << "Connected to the Monitor\n";

    processAttestation(monitorSocket);

    char buffer[SocketUtils::MESSAGE_BYTES];
    string registerString = Messages::REGISTER + " " + Minion::ipAddress;
    General::stringToCharArray(registerString, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);

    if (DebugFlags::debugMinion)
        cout << "Wrote: " << buffer << " to Monitor\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugMinion)
        cout << "Recieved from Monitor: " << buffer << "\n";

    string response(buffer);
    vector<string> splittedResposnse = General::splitString(response);
    if (splittedResposnse[0] == Messages::OK_APPROVED){
        MinionMonitorRequestHandler* minionMonitorRequestHandler = new MinionMonitorRequestHandler(this);
        std::thread monitorRequestHandlerThread(MinionMonitorRequestHandler::startMinionMonitorRequestHandler, minionMonitorRequestHandler);
        monitorRequestHandlerThread.detach();
        return true;
    } else if (splittedResposnse[0] == Messages::NOT_APPROVED){
        return false;
    }
    return false;
}

bool Minion::startAuditingHubHandler(){
    MinionAHubRequestHandler* minionAhubRequestHandler = new MinionAHubRequestHandler(this);
    std::thread ahubRequestHandlerThread(MinionAHubRequestHandler::startMinionAHubRequestHandler, minionAhubRequestHandler);
    ahubRequestHandlerThread.detach();
    return true;
}


int main(int argc, char *argv[]) {
    if (argc != 7) {
        cout << "Usage: Minion -m monitorHostName -h hostname -i ipAddress\n";
        return 0;
    }
    cout << argv[0] << "\n";
    string monitorHostname(argv[MONITOR_HOST_FLAG_INDEX + 1]);
    string hostname(argv[HOSTNAME_FLAG_INDEX + 1]);  //IPADDRESS_FLAG_INDEX
    string ipAddress(argv[IPADDRESS_FLAG_INDEX + 1]);

    std::cout << "Will try to create Minion\n";

     Minion *minion = new Minion(monitorHostname, hostname, ipAddress);

     std::cout << "Bubye\n";

     return 0;
 }