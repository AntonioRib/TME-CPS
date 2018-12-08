#include <iostream>
#include <string>

//  Usage:
//  Client -h: help
//  Client

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Invalid number of fields\n";
        std::cout << "Usage: Client -h\n";
        return -1;
    }

    std::string command = argv[1];
    if (command == "-s")
    {
        std::cout << "This is a template for a client with an endless loop\n";
        std::cout << "Usage: Client -s\n";
    }
    else
    {
        std::cout << "Unknown Command\n";
    }

        std::string line;
        while (true)
        {
            std::getline(std::cin, line);
            if (line == "h")
            {
                std::cout << "You wrote " + line + "\n";
            } else if(line == "q"){
                std::cout << "Bye bye\n";
                break;
            } else {
                std::cout << line + " is not a valid thingie\n";
            }
        }
    return 0;
}
