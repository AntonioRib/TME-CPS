#include <iostream>

//  Usage:
//  Client -h: help
//  Client


int main(int argc, char* argv[])
{
    if(argc != 2){
        std::cout << "Invalid number of fields\n";
        std::cout << "Usage: Client -h\n";
        return -1;
    }

    std::string command = argv[1];
    if(command == "-h") {
        std::cout << "This is a template for a client without a endless loop\n";
        std::cout << "Usage: Client -h\n";
    } else {
        std::cout << "Unknown Command\n";
    }

	return 0;
}
