#include "Minion.h"
#include "Handler/AHubRequestHandler.h"
#include "Handler/MonitorRequestHandler.h"
#include <iterator>
using namespace std;

Minion::Minion()
{
    std::cout << "Minion created\n";
}

Minion::Minion(const Minion &m) : ipAddress{m.ipAddress} {
    // std::cout << "Minion created with the name " << Minion::name << " - copy constructor \n";
}

Minion::Minion(std::string ipAddress) : ipAddress{ipAddress} {
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

//  int main(int argc, char *argv[]) {
//      std::cout << "Will try to create Minion\n";

//      std::string s = "Minion I";
//      Minion* minion = new Minion(s);

//      AHubRequestHandler ahubRequestHandler = AHubRequestHandler(minion);
//      std::thread ahubRequestHandlerThread(AHubRequestHandler::startAHubRequestHandler, ahubRequestHandler);

//      MonitorRequestHandler monitorRequestHandler = MonitorRequestHandler(minion);
//      std::thread monitorRequestHandlerThread(MonitorRequestHandler::startMonitorRequestHandler, monitorRequestHandler);

//      ahubRequestHandlerThread.join();
//      monitorRequestHandlerThread.join();

//      std::cout << "Bubye\n";

//      return 0;
//  }