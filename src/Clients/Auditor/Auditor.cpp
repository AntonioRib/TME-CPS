#include "Auditor.h"
#include "../../Utilities/AttestationConstants.h"
#include "../../Utilities/General.h"
#include "../../Utilities/Messages.h"
#include "../../Utilities/Ports.h"
#include "../../Utilities/httpLib/httplib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

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
    // httplib::Client cli(hostname, Ports::MONITOR_AUDITOR_PORT);

    // auto res = cli.Post(("/" + Messages::ATTEST).c_str(), "", "text/plain");

    // std::vector<std::string> lineSeparated = General::splitString(res.get()->body);

    // std::cout << "Res: " << res.get()->body;

    // if (true)
    //     cli.Post(("/" + Messages::OK_APPROVED).c_str(), AttestationConstants::QUOTE, "text/plain");
    // else {
    //     cli.Post(("/" + Messages::NOT_APPROVED).c_str(), "", "text/plain");
    // }

    int sockfd, n;
    struct sockaddr_in serv_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    struct hostent *server;
    server = gethostbyname(hostname);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(Ports::MONITOR_AUDITOR_PORT);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    // printf("Please enter the message: ");
    char buffer[256];
    bzero(buffer, 256);
    buffer << Messages::ATTEST;

    // fgets(buffer, 255, stdin);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
        error("ERROR writing to socket");

    char quote[1024];
    bzero(quote, 1024);
    n = read(sockfd, buffer, 1023);
    if (n < 0)
        error("ERROR reading from socket");
    printf("QUOTE %s\n", quote);

    buffer << Messages::OK_APPROVED;
    bzero(buffer, 256);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
        error("ERROR writing to socket");
    close(sockfd);
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
