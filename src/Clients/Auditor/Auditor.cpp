#include "Auditor.h"
#include "../../Utilities/AttestationConstants.h"
#include "../../Utilities/General.h"
#include "../../Utilities/Messages.h"
#include "../../Utilities/Ports.h"
#include "../../Utilities/httpLib/httplib.h"

//  Usage:
//  Client -h: help
//  Client

Auditor::Auditor() {
    std::cout << "Auditor created\n";
}

void Auditor::saySomething(std::string message) {
    std::cout << "Auditor >> " + message + "\n";
}

void Auditor::attestMonitor(const char* hostname){
    httplib::Client cli(hostname, Ports::MONITOR_AUDITOR_PORT);

    auto res = cli.Post(("/" + Messages::ATTEST).c_str(), "", "text/plain");

    std::vector<std::string> lineSeparated = General::splitString(res.get()->body);

    std::cout << "Res: " << res.get()->body;

    if (true)
        cli.Post(("/" + Messages::OK_APPROVED).c_str(), AttestationConstants::QUOTE, "text/plain");
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
