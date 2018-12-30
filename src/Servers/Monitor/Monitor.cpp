#include "Monitor.h"
#include "Handler/AHubRequestHandler.h"
#include "Handler/AuditorRequestHandler.h"
#include "Handler/DeveloperRequestHandler.h"
#include "Handler/MinionRequestHandler.h"
using namespace std;

const int USERNAME_FLAG_INDEX = 1;
const int KEY_FLAG_INDEX = 3;
const int HOSTNAME_FLAG_INDEX = 5;

Monitor::Monitor(const Monitor& m) : username{m.username}, sshKey{m.sshKey}, hostname{m.hostname}, 
    trustedMinions{m.trustedMinions}, untrustedMinions{m.untrustedMinions}, applications{m.applications} {
    // std::cout << "Monitor created with the name " << Monitor::hostname << " - copy constructor \n";
}

Monitor::Monitor(string username, string sshKey, string hostName) : username{username}, sshKey{sshKey}, hostname{hostName} {
    std::cout << "Monitor created with the name " << Monitor::username << " - string constructor \n";
    // map<string, Minion> trustedMinions {};
    // map<string, Minion> untrustedMinions {};
    // map<string, string> applications {};
}

Monitor::Monitor() {
    std::cout << "Monitor created\n";
}

int main(int argc, char* argv[]) {


    if(argc != 7){
        cout << "Usage: Monitor -u username -j sshKey -h host\n";
        return 0;
    }

    string username(argv[USERNAME_FLAG_INDEX + 1]);
    string sshKey(argv[KEY_FLAG_INDEX + 1]);
    string hostname(argv[HOSTNAME_FLAG_INDEX + 1]);

    std::cout << "Will try to create Monitor\n";

    std::string s = "Monitor I";
    Monitor monitor = Monitor(username, sshKey, hostname);

    AHubRequestHandler ahubRequestHandler = AHubRequestHandler(monitor);
    std::thread ahubRequestHandlerThread(AHubRequestHandler::startAHubRequestHandler, ahubRequestHandler);

    AuditorRequestHandler auditorRequestHandler = AuditorRequestHandler(monitor);
    std::thread auditorRequestHandlerThread(AuditorRequestHandler::startAuditorRequestHandler, auditorRequestHandler);

    DeveloperRequestHandler developerRequestHandler = DeveloperRequestHandler(monitor);
    std::thread developerRequestHandlerThread(DeveloperRequestHandler::startDeveloperRequestHandler, developerRequestHandler);

    MinionRequestHandler minionRequestHandler = MinionRequestHandler(monitor);
    std::thread minionRequestHandlerThread(MinionRequestHandler::startMinionRequestHandler, minionRequestHandler);

    ahubRequestHandlerThread.join();
    auditorRequestHandlerThread.join();
    developerRequestHandlerThread.join();
    minionRequestHandlerThread.join();

    std::cout << "Bubye\n";

    return 0;
}