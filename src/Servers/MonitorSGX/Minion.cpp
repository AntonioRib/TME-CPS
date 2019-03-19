#include "Minion.h"
#include <iterator>
using namespace std;

// const int MESSAGE_BYTES = 2048;

const int MONITOR_HOST_FLAG_INDEX = 1;
const int HOSTNAME_FLAG_INDEX = 3;
const int IPADDRESS_FLAG_INDEX = 5;


Minion::Minion()
{
    std::cout << "Minion created\n";
}

Minion::Minion(const Minion &m) : ipAddress{m.ipAddress} {
    // std::cout << "Minion created with the name " << Minion::name << " - copy constructor \n";
}

Minion::Minion(string ipAddress) : ipAddress{ipAddress} {
    // std::cout << "Minion created with the name " << Minion::name << " - string constructor \n";
}

string Minion::getIpAddress(){
    return Minion::ipAddress;
}
map<string, Application *> Minion::getApplications(){
    return Minion::minionApplications;
}

void Minion::addApp(Application *app){
    Minion::minionApplications.insert(pair<string, Application*>(app->getAppID(), app));
}

void Minion::removeApp(string appID){
    Minion::minionApplications.erase(appID);
}