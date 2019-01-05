#include "AuditorRequestHandler.h"

// #include <arpa/inet.h>
// #include <openssl/bio.h>
// #include <openssl/err.h>
// #include <openssl/ssl.h>
// #include <sys/socket.h>
// #include <openssl/applink.c>

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

void AuditorRequestHandler::processAttestation(int socket, std::string nonce, Monitor& monitor)
{
    unsigned char *uc = (unsigned char *)"LALA";
    monitor.setApprovedConfiguration(uc);
    // svr.Post(("/" + Messages::OK_APPROVED).c_str(), [&](const httplib::Request &req, httplib::Response &res) {
    //         std::cout << "Recieved an OK. Going to update configuration. \n";
    //     unsigned char *uc = (unsigned char *)req.body.c_str();
    //     monitor.setApprovedConfiguration(uc);
    //         std::cout << "Configuration was set. \n";
    // });

    // svr.Get(("/" + Messages::NOT_APPROVED).c_str(), [&](const httplib::Request &req, httplib::Response &res) {
    //         std::cout << "Recieved an NOT_APPROVED. Going to close. \n";
    //     svr.stop();
    // });
}

void AuditorRequestHandler::startAuditorRequestHandler(AuditorRequestHandler auditorRequestHandler) {
    std::cout << "Trying to create AuditorRequestHandler with Monitor with the name " << auditorRequestHandler.monitor->getHostname() << "\n";

    // httplib::Server svr;

    int sockfd, newsockfd, port;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        cout << "ERROR opening socket";

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(Ports::MONITOR_AUDITOR_PORT);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
        error("ERROR on binding");

    listen(sockfd, 5);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    printf("server: got connection from %s port %d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
    
    // send(newsockfd, "Hello, world!\n", 13, 0);

    bzero(buffer, 256);
    int n;
    n = read(newsockfd, buffer, 255);
    if (n < 0)
        error("ERROR reading from socket");

    cout << buffer;
    string command(buffer);
    vector<string> commandSplit = General::splitString(command);
    if (commandSplit[0] == Messages::ATTEST){
        auditorRequestHandler.processAttestation(sockfd, AttestationConstants::QUOTE, *(auditorRequestHandler.monitor));
        std::string configuration = Messages::QUOTE + " " + AttestationConstants::QUOTE;
        send(newsockfd, configuration.c_str(), 1023, 0);

        bzero(buffer, 256);
        n = read(newsockfd, buffer, 255);
        cout << buffer;
        string approved(buffer);
        if (approved == Messages::OK_APPROVED){
            cout << "Approved!\n";
        }
    }
        // printf("Here is the message: %s\n", buffer);

    close(newsockfd);
    close(sockfd);
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