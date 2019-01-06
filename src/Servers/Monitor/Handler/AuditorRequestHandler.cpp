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

void AuditorRequestHandler::processAttestation(int clientSocket, std::string nonce, Monitor& monitor)
{
    char buffer[MESSAGE_BYTES];
    bzero(buffer, MESSAGE_BYTES);
    int n;
    std::string configuration = Messages::QUOTE + " " + AttestationConstants::QUOTE;
    strncpy(buffer, configuration.c_str(), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = 0;
    n = send(clientSocket, buffer, strlen(buffer), 0);
    if (n < 0)
        error("ERROR writing to socket");
    cout << "Wrote: " << buffer << " to client\n";

    bzero(buffer, MESSAGE_BYTES);
    n = recv(clientSocket, buffer, MESSAGE_BYTES - 1, 0);
    if (n < 0)
        error("ERROR reading from socket");
    cout << "Recieved: " << buffer << "\n";
    // cout << buffer;
    string approved(buffer);
    vector<string> approvedSplit = General::splitString(approved);

    if (approvedSplit[0] == Messages::NOT_APPROVED) {
        cout << "Not approved!\n";
    } else if (approvedSplit[0] == Messages::OK_APPROVED) {
        cout << "Approved!\n"
        cout << "Configuration approved. Auditor signature for monitor: " + approvedSplit[1] + ". For minions:" + approvedSplit[3];
        unsigned char * approvedConfiguration = (unsigned char *) approvedSplit[1].c_str();
        monitor.setApprovedConfiguration(approvedConfiguration, approvedSplit[2]);
        monitor.setApprovedConfigurationForMinions(approvedSplit[3]);
    }
}

void AuditorRequestHandler::startAuditorRequestHandler(AuditorRequestHandler auditorRequestHandler) {
    std::cout << "Trying to create AuditorRequestHandler with Monitor with the name " << auditorRequestHandler.monitor->getHostname() << "\n";

    int serverSocket;
    struct sockaddr_in serverAddress;
    bzero((char *)&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddress.sin_port = htons(Ports::MONITOR_AUDITOR_PORT);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
        error("ERROR opening socket");

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) 
        error("ERROR on binding");

    listen(serverSocket, 5);

    int clientSocket;
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength = sizeof(clientAddress);
    while(true){
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddress, &clientAddressLength);
        if (clientSocket < 0)
            error("ERROR on accept");

        cout << "Got connection from client\n";
        //cout << "server: got connection from " << inet_ntoa(clientAddress.sin_addr) << " port " + ntohs(clientAddress.sin_port) << "\n";

        // send(clientSocket, "Hello, world!\n", 13, 0);
        char buffer[MESSAGE_BYTES];
        bzero(buffer, MESSAGE_BYTES);
        int n;
        n = recv(clientSocket, buffer, MESSAGE_BYTES - 1, 0);
        if (n < 0)
            error("ERROR reading from socket");
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