#include "AuditorRequestHandler.h"
#include "../../../Utilities/Messages.h"
#include "../../../Utilities/AttestationConstants.h"
#include "../../../Utilities/Ports.h"
#include "../../../Utilities/httpLib/httplib.h"
// #include <arpa/inet.h>
// #include <openssl/bio.h>
// #include <openssl/err.h>
// #include <openssl/ssl.h>
// #include <sys/socket.h>
// #include <openssl/applink.c>

AuditorRequestHandler::AuditorRequestHandler() {
    std::cout << "AuditorRequestHandler created\n";
}

AuditorRequestHandler::AuditorRequestHandler(Monitor* monitor) : monitor{monitor}, monitorStore{monitor->getHostname()+".jks"}{
    std::cout << "AuditorRequestHandler created with the name " << monitor->getHostname() << "\n";
}

void processAttestation(httplib::Server svr, std::string nonce, Monitor monitor){
        // svr.
}

void AuditorRequestHandler::startAuditorRequestHandler(AuditorRequestHandler auditorRequestHandler) {
    std::cout << "Trying to create AuditorRequestHandler with Monitor with the name " << auditorRequestHandler.monitor->getHostname() << "\n";

    httplib::Server svr;
    bool debug = true;

    svr.Post(("/" + Messages::ATTEST).c_str(), [&](const httplib::Request& req, httplib::Response& res) {
        if(debug)
            std::cout << "Recieved an ATTEST. Going to send configuration. \n";
        std::string configuration = Messages::QUOTE + " " + AttestationConstants::QUOTE;
        res.set_content(configuration, "text/plain");
    });

    svr.Post(("/" + Messages::OK_APPROVED).c_str(), [&](const httplib::Request& req, httplib::Response& res) {
        if (debug)
            std::cout << "Recieved an OK. Going to update configuration. \n";
        unsigned char* uc = (unsigned char*)req.body.c_str();
        auditorRequestHandler.monitor->setApprovedConfiguration(uc);
        if (debug)
            std::cout << "Configuration was set. \n";
    });

    svr.Get(("/" + Messages::NOT_APPROVED).c_str(), [&](const httplib::Request& req, httplib::Response& res) {
        if (debug)
            std::cout << "Recieved an NOT_APPROVED. Going to close. \n";
        svr.stop();
    });

    svr.Get(("/" + Messages::NOT_OK).c_str(), [&](const httplib::Request& req, httplib::Response& res) {
        res.set_content(Messages::NOT_OK, "text/plain");
        svr.stop();
    });
    if (debug)
        std::cout << "AuditorRequestHandler running with Monitor with the name " << auditorRequestHandler.monitor->getHostname() << "\n";
    svr.listen(auditorRequestHandler.monitor->getHostname().c_str(), Ports::MONITOR_AUDITOR_PORT);
    if (debug)
        std::cout << "AuditorRequestHandler finished \n";
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create AuditorRequestHandler\n";

//     AuditorRequestHandler* auditorRequestHandler;
//     auditorRequestHandler = new AuditorRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }