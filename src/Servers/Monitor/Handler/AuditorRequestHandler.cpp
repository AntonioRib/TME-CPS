#include "AuditorRequestHandler.h"

// #include <arpa/inet.h>
// #include <openssl/bio.h>
// #include <openssl/err.h>
// #include <openssl/ssl.h>
// #include <sys/socket.h>
// #include <openssl/applink.c>

const int MESSAGE_BYTES = 2048;

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

AuditorRequestHandler::AuditorRequestHandler() {
    std::cout << "AuditorRequestHandler created\n";
}

AuditorRequestHandler::AuditorRequestHandler(Monitor* monitor) : monitor{monitor}, monitorStore{monitor->getHostname()+".jks"}{
    std::cout << "AuditorRequestHandler created with the name " << monitor->getHostname() << "\n";
}

void AuditorRequestHandler::processAttestation(int clientSocket, std::string nonce, Monitor& monitor){
    char buffer[MESSAGE_BYTES];
    std::string configuration = Messages::QUOTE + " " + AttestationConstants::QUOTE;
    General::stringToCharArray(configuration, buffer, MESSAGE_BYTES);
    // cout << "Buffer: " << buffer;
    SocketUtils::sendBuffer(clientSocket, buffer, strlen(buffer), 0);
    if (DebugFlags::debugMonitor)
        cout << "Wrote: " << buffer << " to client\n";

    bzero(buffer, MESSAGE_BYTES);
    SocketUtils::receiveBuffer(clientSocket, buffer, MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugMonitor)
        cout << "Recieved: " << buffer << "\n";

    string approved(buffer);
    vector<string> approvedSplit = General::splitString(approved);

    if (approvedSplit[0] == Messages::NOT_APPROVED) {
        cout << "Not approved!\n";
    } else if (approvedSplit[0] == Messages::OK_APPROVED) {
        cout << "Approved!\n";
        cout << "Configuration approved. Auditor signature for monitor: " + approvedSplit[1] + ". For minions:" + approvedSplit[3];
        unsigned char * approvedConfiguration = (unsigned char *) approvedSplit[1].c_str();
        monitor.setApprovedConfiguration(approvedConfiguration, approvedSplit[2]);
        monitor.setApprovedConfigurationForMinions(approvedSplit[3]);
    }
}

void AuditorRequestHandler::startAuditorRequestHandler(AuditorRequestHandler auditorRequestHandler) {
    std::cout << "Trying to create AuditorRequestHandler with Monitor with the name " << auditorRequestHandler.monitor->getHostname() << "\n";

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::MONITOR_AUDITOR_PORT);

    int serverSocket;
    serverSocket = SocketUtils::createServerSocket(serverAddress);

    int clientSocket;
    while(true){
        clientSocket = SocketUtils::acceptClientSocket(serverSocket);
        cout << "Got connection from client\n";

        char buffer[MESSAGE_BYTES];
        bzero(buffer, MESSAGE_BYTES);
        SocketUtils::receiveBuffer(clientSocket, buffer, MESSAGE_BYTES - 1, 0);
        if (DebugFlags::debugMonitor)
            cout << "Recieved: " << buffer << "\n";
        // cout << buffer;
        string command(buffer);
        vector<string> commandSplit = General::splitString(command);
        
        if (commandSplit[0] == Messages::ATTEST){
            auditorRequestHandler.processAttestation(clientSocket, AttestationConstants::QUOTE, *(auditorRequestHandler.monitor));
        }
    }
    // printf("Here is the message: %s\n", buffer);

    close(clientSocket);
    close(serverSocket);
    //return 0;

    // bool debug = true;

    // //Starts with the Attest command
    // svr.Post(("/" + Messages::ATTEST).c_str(), [&](const httplib::Request& req, httplib::Response& res) {
    //     if(debug)
    //         std::cout << "Recieved an ATTEST. Going to send configuration. \n";
    //     std::string configuration = Messages::QUOTE + " " + AttestationConstants::QUOTE;
    //     auditorRequestHandler.processAttestation(svr, AttestationConstants::QUOTE, *(auditorRequestHandler.monitor));
    //     res.set_content(configuration, "text/plain");
    // });

    // svr.Get(("/" + Messages::NOT_OK).c_str(), [&](const httplib::Request& req, httplib::Response& res) {
    //     res.set_content(Messages::NOT_OK, "text/plain");
    //     svr.stop();
    // });
    // if (debug)
    //     std::cout << "AuditorRequestHandler running with Monitor with the name " << auditorRequestHandler.monitor->getHostname() << "\n";
    // svr.listen(auditorRequestHandler.monitor->getHostname().c_str(), Ports::MONITOR_AUDITOR_PORT);
    // if (debug)
    //     std::cout << "AuditorRequestHandler finished \n";
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create AuditorRequestHandler\n";

//     AuditorRequestHandler* auditorRequestHandler;
//     auditorRequestHandler = new AuditorRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }