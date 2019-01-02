#include "Developer.h"

//  Usage:
//  Developer -h: help
//  Developer

Developer::Developer() {
    std::cout << "Developer created\n";
}

void Developer::attestMonitor(){
    //TODO
}

bool Developer::sendSyncMessageAndGetResponse(string message){
    //TODO
}

bool Developer::deleteApp(){
    //TODO
}

bool Developer::sendApp(){
    //TODO
}

bool Developer::deployApp(){
    //TODO
}

void Developer::saySomething(std::string message) {
    std::cout << "Developer >> " + message + "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Invalid number of fields\n";
        std::cout << "Usage: Developer -h\n";
        std::cout << "Usage: Developer -c\n";
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

    Developer* developer;
    developer = new Developer();

    std::string line;
    while (true) {
        std::getline(std::cin, line);
        if (line == "exit") {
            std::cout << "Bye bye\n";
            break;
        }
        developer->saySomething(line);
    }
    return 0;
}