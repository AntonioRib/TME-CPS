
#include "AuditorInterface.h"
using namespace std;

const int MONITOR_FLAG_INDEX = 1;
const int HOSTNAME_FLAG_INDEX = 3;

const string UNCOMMITED_LOGS_DIR = "Logs/Uncommited/";
const string COMMITED_LOGS_DIR = "Logs/Commited/";
//  Usage:
//  Client -h: help
//  Client
AuditorInterface::AuditorInterface() {
    std::cout << "AuditorInterface created\n";
}

AuditorInterface::AuditorInterface(string monitorHost, string hostname) : monitorHost{monitorHost}, hostname{hostname} {
    std::cout << "AuditorInterface created\n";
}

bool AuditorInterface::setTrusted(string hostname) {
    struct hostent* serverHost;
    serverHost = SocketUtils::getHostByName(hostname);

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::MONITOR_AHUB_PORT);

    int monitorSocket = socket(AF_INET, SOCK_STREAM, 0);

    SocketUtils::connectToServerSocket(monitorSocket, serverAddress);
    if (DebugFlags::debugAuditor)
        cout << "Connected to the server\n";

    char buffer[SocketUtils::MESSAGE_BYTES];
    string host(serverHost->h_name);
    string requestString = Messages::SET_TRUSTED + " " + host;
    General::stringToCharArray(requestString, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(monitorSocket, buffer, strlen(buffer), 0);

    if (DebugFlags::debugAuditor)
        cout << "Wrote: " << buffer << " to server\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(monitorSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugAuditor)
        cout << "Recieved from server: " << buffer << "\n";

    string response(buffer);
    vector<string> splittedResponse = General::splitString(response);

    if (splittedResponse[0] == Messages::OK) {
        if (DebugFlags::debugAuditor)
            cout << "Success on setting node as trusted\n";
        close(monitorSocket);
        return true;
    } else if (splittedResponse[0] == Messages::NOT_OK) {
        close(monitorSocket);
        return false;
    }
    return false;
}

void printHelp() {
    std::cout << "Usage: AuditorInterface -m monitor -h hostName\n";
}

inline bool ends_with(std::string const& value, std::string const& ending) {
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

void printAvaliableCommands() {
    std::cout << "------------------\n";
    std::cout << "Avaliable Commands:\n";
    std::cout << "Usage: Help - help\n";
    std::cout << "Usage: List Uncommited Logs - lu\n";
    std::cout << "Usage: List Commited Logs - lc\n";
    std::cout << "Usage: Read Uncommited Log - r log_id\n";
    std::cout << "Usage: Commit Log - c log_id\n";
    std::cout << "Usage: Quit - exit\n";
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cout << "Invalid number of fields\n";
        printHelp();
        return -1;
    }

    string monitorhost = argv[MONITOR_FLAG_INDEX + 1];
    string hostname = argv[HOSTNAME_FLAG_INDEX + 1];

    AuditorInterface* auditorInterface;
    auditorInterface = new AuditorInterface(monitorhost, hostname);
    // printHelp();
    //CREATE LOG FOLDER
    if (mkdir(UNCOMMITED_LOGS_DIR.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1)
        cout << "Directory created";

    if (mkdir(COMMITED_LOGS_DIR.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != -1)
        cout << "Directory created";
    
    printAvaliableCommands();
    std::string line;
    while (true) {
        std::cout << "------------------\n";
        cout << "> ";
        std::getline(std::cin, line);
        std::vector<std::string> lineSeparated = General::splitString(line);

        if (line == "exit") {
            std::cout << "Bye bye\n";
            break;
        } else if (line == "help") {
            printAvaliableCommands();
        } else if (lineSeparated[0] == "lu") {
            std::string path = UNCOMMITED_LOGS_DIR;
            DIR* dir;
            struct dirent* ent;
            if ((dir = opendir(path.c_str())) != NULL) {
                /* print all the files and directories within directory */
                while ((ent = readdir(dir)) != NULL) {
                   if (ends_with(ent->d_name, ".log"))
                    cout << ent->d_name << "\n";
                }
                closedir(dir);
            } else {
                /* could not open directory */
                perror("");
                return EXIT_FAILURE;
            }
        } else if (lineSeparated[0] == "lc") {
            std::string path = COMMITED_LOGS_DIR;
            DIR* dir;
            struct dirent* ent;
            if ((dir = opendir(path.c_str())) != NULL) {
                /* print all the files and directories within directory */
                while ((ent = readdir(dir)) != NULL) {
                    if (ends_with(ent->d_name, ".log"))
                        cout << ent->d_name << "\n";
                }
                closedir(dir);
            } else {
                /* could not open directory */
                perror("");
                return EXIT_FAILURE;
            }
        } else if (lineSeparated[0] == "r") {
            ifstream readFile;
            readFile.open(UNCOMMITED_LOGS_DIR + lineSeparated[1]);
            char output[100];
            if (readFile.is_open()) {
                while (!readFile.eof()) {
                    readFile >> output;
                    cout << output;
                    cout << "\n";
                }
            } else {
                cout << "Log not found\n";
            }
            readFile.close();
        } else if (lineSeparated[0] == "c") {
            string oldName = UNCOMMITED_LOGS_DIR + lineSeparated[1];
            string newName = COMMITED_LOGS_DIR + lineSeparated[1];
            ifstream readFile;
            readFile.open(oldName);
            if (!readFile.is_open()){
                cout << "Log not found\n";
            } else {
                readFile.close();
                    //SET NODE TRUSTED
            
                std::string delimiter = ".";
                std::string hostname = lineSeparated[1].substr(0, lineSeparated[1].find(delimiter));

                if (!auditorInterface->setTrusted(hostname))
                    cout << "Failed to set not to trusted";
                else {
                    ifstream ifs(oldName, ios::in | ios::binary);
                    ofstream ofs(newName, ios::out | ios::binary);
                    ofs << ifs.rdbuf();
                    remove(oldName.c_str());
                    cout << "Log commited\n";
                }
            }
        } else {
            std::cout << "Unknown command \n";
        }
        // auditor->saySomething(line);
    }
    return 0;
}
