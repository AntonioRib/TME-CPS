#include "SysAdmin.h"

//  Usage:
//  SysAdmin -h: help
//  SysAdmin

SysAdmin::SysAdmin() {
    std::cout << "Auditor created\n";
}

void SysAdmin::saySomething(std::string message) {
    while (true) {
        std::cout << "Auditor >> " + message + "\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    }
}

std::thread SysAdmin::saySomethingThread(std::string message) {
    return std::thread([this, message] { saySomething(message); });
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
    std::thread t1;
    std::string line;
    std::getline(std::cin, line);
    t1 = sysAdmin->saySomethingThread(line);
    while (true) {
        if (line == "exit") {
            std::cout << "Bye bye\n";
            break;
        }
        std::getline(std::cin, line);
    }
    t1.join();
    return 0;
}
