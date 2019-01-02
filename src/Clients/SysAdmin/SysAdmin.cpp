#include "SysAdmin.h"

//  Usage:
//  SysAdmin -h: help
//  SysAdmin

SysAdmin::SysAdmin() {
    std::cout << "Auditor created\n";
}

void SysAdmin::attestLogger(){
    //TODO
} 

bool SysAdmin::starLocalProxy(){
    //TODO
}

bool SysAdmin::manageNode(){
    //TODO
}

void SysAdmin::saySomething(std::string message) {
        std::cout << "Auditor >> " + message + "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Invalid number of fields\n";
        std::cout << "Usage: SysAdmin -h\n";
        std::cout << "Usage: SysAdmin -c\n";
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

    SysAdmin* sysAdmin;
    sysAdmin = new SysAdmin();
    std::string line;
    std::getline(std::cin, line);
    while (true) {
        if (line == "exit") {
            std::cout << "Bye bye\n";
            break;
        }
        std::getline(std::cin, line);
    }
    return 0;
}
