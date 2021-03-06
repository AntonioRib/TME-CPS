#include "AuditorMinion.h"
#include "../../Utilities/AttestationConstants.h"
#include "../../Utilities/General.h"
#include "../../Utilities/Messages.h"
#include "../../Utilities/Ports.h"
using namespace std;

// const int MESSAGE_BYTES = 2048;
//  Usage:
//  Client -h: help
//  Client
Auditor::Auditor() {
    std::cout << "Auditor created\n";
}
void Auditor::attestMinion(const char* hostname) {
    // cout << "got inside the method going to try to connect \n";
    struct hostent* serverHost;
    serverHost = SocketUtils::getHostByName(hostname);

    sockaddr_in serverAddress;
    serverAddress = SocketUtils::createServerAddress(Ports::MINION_AUDITOR_PORT + 20);
    bcopy((char*)serverHost->h_addr, (char*)&serverAddress.sin_addr.s_addr, serverHost->h_length);

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    SocketUtils::connectToServerSocket(clientSocket, serverAddress);
    if (DebugFlags::debugAuditor)
        cout << "Connected to the server\n";

    char buffer[SocketUtils::MESSAGE_BYTES];
    string attestationRequestString = Messages::ATTEST + " " + AttestationConstants::NONCE;
    General::stringToCharArray(attestationRequestString, buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::sendBuffer(clientSocket, buffer, strlen(buffer), 0);

    if (DebugFlags::debugAuditor)
        cout << "Wrote: " << buffer << " to server\n";

    bzero(buffer, SocketUtils::MESSAGE_BYTES);
    SocketUtils::receiveBuffer(clientSocket, buffer, SocketUtils::MESSAGE_BYTES - 1, 0);
    if (DebugFlags::debugAuditor)
        cout << "Recieved from server: " << buffer << "\n";

    string quote(buffer);
    vector<string> splittedQuote = General::splitString(quote);

    if (splittedQuote[0] == Messages::QUOTE && splittedQuote[1] == AttestationConstants::QUOTE) {
        string approvedMessage = Messages::OK_APPROVED + " "; //+ AttestationConstants::PCR_SHA1 + " " + AttestationConstants::PCR_SHA1 + " " + AttestationConstants::PCR_SHA1;
        General::stringToCharArray(approvedMessage, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(clientSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugAuditor)
            cout << "Wrote: " << buffer << " to server\n";
        close(clientSocket);
    } else {
        General::stringToCharArray(Messages::NOT_OK, buffer, SocketUtils::MESSAGE_BYTES);
        SocketUtils::sendBuffer(clientSocket, buffer, strlen(buffer), 0);
        if (DebugFlags::debugAuditor)
            cout << "Wrote: " << buffer << " to server\n";
        close(clientSocket);
    }
    //return 0;
}

unsigned char* Auditor::generateSignature(unsigned char* data){
    //TODO
}

void Auditor::saySomething(std::string message) {
    std::cout << "Auditor >> " + message + "\n";
}

void printHelp(){
    std::cout << "Usage: Help - help\n";
    std::cout << "Usage: Attest Minion - attm ip\n";
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
        try {
            std::cout << ">> ";
            std::getline(std::cin, line);
            std::vector<std::string> lineSeparated = General::splitString(line);

            if (line == "exit") {
                std::cout << "Bye bye\n";
                break;
            } else if (line == "help"){
                printHelp();
            } else if (lineSeparated[0] == "attm") {
                if(lineSeparated.size() == 2){
                    if(DebugFlags::debugAuditor)
                        std::cout << "Command " << lineSeparated[0] << " " << lineSeparated[1] << "\n";
                    auditor->attestMinion(lineSeparated[1].c_str());
                } else {
                    std::cout << "Bad usage of command \n";
                    printHelp();
                }
            } else {
                std::cout << "Unknown command \n";
            }
            // auditor->saySomething(line);
        } catch (const exception& e){
                cout << e.what() << '\n';
        }
    }
    return 0;
}
