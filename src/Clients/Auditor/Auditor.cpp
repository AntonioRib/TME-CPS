#include "Auditor.h"

//  Usage:
//  Client -h: help
//  Client

Auditor::Auditor() {
    std::cout << "Auditor created\n";
}

void Auditor::saySomething(std::string message) {
    std::cout << "Auditor >> " + message + "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Invalid number of fields\n";
        std::cout << "Usage: Auditor -h\n";
        std::cout << "Usage: Auditor -c\n";
        return -1;
    }

    std::string command = argv[1];
    if (command == "-h") {
        std::cout << "This is an Auditor\n";
        std::cout << "Usage: Auditor -h\n";
        std::cout << "Usage: Auditor -c\n";
    } else if (command != "-c") {
        std::cout << "Unknown Command\n";
        return -1;
    }

    Auditor* auditor;
    auditor = new Auditor();

    std::string line;
    while (true) {
        std::getline(std::cin, line);
        if (line == "exit") {
            std::cout << "Bye bye\n";
            break;
        }
        auditor->saySomething(line);
    }
    return 0;
}
