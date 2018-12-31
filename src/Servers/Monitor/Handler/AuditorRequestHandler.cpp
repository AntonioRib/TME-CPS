#include "AuditorRequestHandler.h"
#include "../../../Utilities/Messages.h"
#include "../../../Utilities/httpLib/httplib.h"
#include "../../../Utilities/Ports.h"
// #include <arpa/inet.h>
// #include <openssl/bio.h>
// #include <openssl/err.h>
// #include <openssl/ssl.h>
// #include <sys/socket.h>
// #include <openssl/applink.c>

AuditorRequestHandler::AuditorRequestHandler() {
    std::cout << "AuditorRequestHandler created\n";
}

AuditorRequestHandler::AuditorRequestHandler(Monitor* monitor) : monitor{monitor} {
    std::cout << "AuditorRequestHandler created with the name " << monitor->hostname << "\n";
}

void processAttestation(httplib::Server svr, std::string nonce, Monitor monitor){
        // svr.
}

void AuditorRequestHandler::startAuditorRequestHandler(AuditorRequestHandler auditorRequestHandler) {
    std::cout << "Trying to create AuditorRequestHandler with Monitor with the name " << auditorRequestHandler.monitor->hostname << "\n";

    httplib::Server svr;
    bool debug = false;

    svr.Get(("/" + Messages::OK).c_str(), [&](const httplib::Request& req, httplib::Response& res) {
        res.set_content(Messages::OK, "text/plain");
        std::cout << "Recieved an OK. Going to set configuration. \n";
        auditorRequestHandler.monitor->setApprovedConfiguration(true);
        std::cout << "Configuration was set. \n";
    });

    svr.Get(("/" + Messages::NOT_OK).c_str(), [&](const httplib::Request& req, httplib::Response& res) {
        res.set_content(Messages::NOT_OK, "text/plain");
        svr.stop();
    });

    std::cout << "AuditorRequestHandler running with Monitor with the name " << auditorRequestHandler.monitor->hostname << "\n";
    svr.listen(auditorRequestHandler.monitor->hostname.c_str(), Ports::MONITOR_AUDITOR_PORT);
    std::cout << "AuditorRequestHandler finished " << "\n";
}

// int main(int argc, char* argv[]) {
//     std::cout << "Will try to create AuditorRequestHandler\n";

//     AuditorRequestHandler* auditorRequestHandler;
//     auditorRequestHandler = new AuditorRequestHandler();

//     std::cout << "Bubye\n";

//     return 0;
// }