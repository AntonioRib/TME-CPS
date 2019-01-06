#include "Auditor.h"
#include "../../Utilities/AttestationConstants.h"
#include "../../Utilities/General.h"
#include "../../Utilities/Messages.h"
#include "../../Utilities/Ports.h"
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;

const int MESSAGE_BYTES = 2048;
//  Usage:
//  Client -h: help
//  Client

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

Auditor::Auditor() {
    std::cout << "Auditor created\n";
}

void Auditor::attestMonitor(const char* hostname){
    // cout << "got inside the method going to try to connect \n";
    int n;

    struct hostent *serverHost;
    serverHost = gethostbyname(hostname);
    if (serverHost == NULL)
        error("ERROR, no such host\n");

    int serverSocket;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
        error("ERROR opening socket");

    sockaddr_in serverAddress;
    bzero((char *)&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    bcopy((char *)serverHost->h_addr, (char *)&serverAddress.sin_addr.s_addr, serverHost->h_length);
    serverAddress.sin_port = htons(Ports::MONITOR_AUDITOR_PORT);
    if (connect(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
        error("ERROR connecting");
    
    cout << "Connected to the server\n";

    char buffer[MESSAGE_BYTES];
    bzero(buffer, MESSAGE_BYTES);

    string attestationRequestString = Messages::ATTEST + " " + AttestationConstants::NONCE;

    strncpy(buffer, attestationRequestString.c_str(), sizeof(buffer));
    buffer[sizeof(buffer) - 1] = 0;

    // fgets(buffer, 255, stdin);
    n = send(serverSocket, buffer, strlen(buffer), 0);
    if (n < 0)
        error("ERROR writing to socket");
    // cout << "message sent \n";
    cout << "Wrote: " << buffer << " to server\n";
    // cout << "Will recieve message \n";
    bzero(buffer, MESSAGE_BYTES);
    n = recv(serverSocket, buffer, MESSAGE_BYTES-1, 0);
    if (n < 0)
        error("ERROR reading from socket");
    cout << "Recieved in BUFFER: " << buffer << "\n";
    string quote(buffer);
    vector<string> splittedQuote = General::splitString(quote);

    if (splittedQuote[0] == Messages::QUOTE && splittedQuote[1] == AttestationConstants::QUOTE) {
        bzero(buffer, MESSAGE_BYTES);
        string approvedMessage = Messages::OK_APPROVED + " " + AttestationConstants::PCR_SHA1 + " " + AttestationConstants::PCR_SHA1 + " " + AttestationConstants::PCR_SHA1;
        strncpy(buffer, approvedMessage.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = 0;
        n = send(serverSocket, buffer, strlen(buffer), 0);
        if (n < 0)
            error("ERROR writing to socket");
        cout << "Wrote: " << buffer << " to server\n";
        close(serverSocket);
    } else {
        bzero(buffer, MESSAGE_BYTES);
        strncpy(buffer, Messages::NOT_OK.c_str(), sizeof(buffer));
        buffer[sizeof(buffer) - 1] = 0;
        n = send(serverSocket, buffer, strlen(buffer), 0);
        if (n < 0)
            error("ERROR writing to socket");
        cout << "Wrote: " << buffer << " to server\n";
        close(serverSocket);
    }

    
    //return 0;
}

void Auditor::attestLogger(const char* hostname){
    //TODO
}
unsigned char* Auditor::generateSignature(unsigned char* data){
    //TODO
}

void Auditor::saySomething(std::string message) {
    std::cout << "Auditor >> " + message + "\n";
}



void printHelp(){
    std::cout << "Usage: Help - help\n";
    std::cout << "Usage: Attest Monitor - attm monitor_host\n";
    std::cout << "Usage: Attest Logger - attl logger_host\n";
    std::cout << "Usage: Exit - exit\n";
}

int main(int argc, char* argv[]) {
    if (argc != 1) {
        std::cout << "Invalid number of fields\n";
        //printHelp();
        return -1;
    }

    Auditor* auditor;
    auditor = new Auditor();
    printHelp();

    std::string line;
    
    while (true) {
        std::getline(std::cin, line);
        std::vector<std::string> lineSeparated = General::splitString(line);

        if (line == "exit") {
            std::cout << "Bye bye\n";
            break;
        } else if (line == "help"){
            printHelp();
        } else if (lineSeparated[0] == "attm") {
            if(lineSeparated.size() == 2){
                std::cout << "Command " << lineSeparated[0] << " " << lineSeparated[1] << "\n";
                auditor->attestMonitor(lineSeparated[1].c_str());
            } else {
                std::cout << "Bad usage of command \n";
                printHelp();
            }
        } else if (lineSeparated[0] == "attl") {
            if (lineSeparated.size() == 2) {
                std::cout << "Command " << lineSeparated[0] << " " << lineSeparated[1];
            } else {
                std::cout << "Bad usage of command \n";
                printHelp();
            }
        } else {
            std::cout << "Unknown command \n";
        }
        // auditor->saySomething(line);
    }
    return 0;
}
