#include "Monitor.h"
#include <algorithm>
#include <chrono>
#include <iterator>
#include "Handler/AHubRequestHandler.h"
#include "Handler/AuditorRequestHandler.h"
#include "Handler/DeveloperRequestHandler.h"
#include "Handler/MinionRequestHandler.h"

using namespace std;

const int USERNAME_FLAG_INDEX = 1;
const int KEY_FLAG_INDEX = 3;
const int HOSTNAME_FLAG_INDEX = 5;

// const unsigned char* NULL_VALUE = (unsigned char*)"\0";

//Constructors
Monitor::Monitor(const Monitor& m) : username{m.username}, sshKey{m.sshKey}, hostname{m.hostname}, 
    trustedMinions{m.trustedMinions}, untrustedMinions{m.untrustedMinions}, applications{m.applications} {
    Monitor::approvedConfiguration = (unsigned char*)General::NULL_VALUE;  //std::nullptr_t();
    // std::cout << "Monitor created with the name " << Monitor::hostname << " - copy constructor \n";
}

Monitor::Monitor(string username, string sshKey, string hostName) : username{username}, sshKey{sshKey}, hostname{hostName} {
    std::cout << "Monitor created with the name " << Monitor::username << " - string constructor \n";
    Monitor::approvedConfiguration = (unsigned char*)General::NULL_VALUE;
    // map<string, Minion> trustedMinions {};
    // map<string, Minion> untrustedMinions {};
    // map<string, string> applications {};
}

Monitor::Monitor() {
    std::cout << "Monitor created\n";
}

//Getters
string Monitor::getSSHKey(){
    return Monitor::sshKey;
}

string Monitor::getUsername(){
    return Monitor::username;
}

string Monitor::getHostname(){
    return Monitor::hostname;
}

vector<Minion*> Monitor::getHosts(string appID){
    return appsHosts[appID];
}

map<string, Minion*> Monitor::getUntrustedMinions() {
    // vector<Minion*> result;
    // General::mapToVec <map<string,Minion*>, vector<Minion*>> (Monitor::untrustedMinions, result);
    // return result;
    return Monitor::untrustedMinions;
}

map<string, Minion*> Monitor::getTrustedMinions() {
    // vector<Minion*> result;
    // General::mapToVec<map<string, Minion*>, vector<Minion*>>(Monitor::trustedMinions, result);
    // return result;
    return Monitor::trustedMinions;
}

unsigned char* Monitor::getApprovedConfiguration() {
    return Monitor::approvedConfiguration;
}

string Monitor::getApprovedSHA1() {
    return Monitor::approvedSHA1;
}

string Monitor::getApprovedConfigurationForMinions() {
    return Monitor::minionsApprovedSHA1;
}

//Setters
void Monitor::setApprovedConfiguration(unsigned char* approvedConfiguration, string pcrSHA1){
    std::cout << "Updated approvedConfiguration from: " << +Monitor::approvedConfiguration;
    Monitor::approvedConfiguration = approvedConfiguration;
    std::cout << " to: " << +Monitor::approvedConfiguration << "\n";
    std::cout << "Updated pcrSHA1 from: " << Monitor::approvedSHA1;
    Monitor::approvedSHA1 = pcrSHA1;
    std::cout << " to: " << Monitor::approvedSHA1 << "\n";
}

void Monitor::setApprovedConfigurationForMinions(string approvedConfiguration){
    Monitor::minionsApprovedSHA1 = approvedConfiguration;
}

//Special Getters
Minion* Monitor::getUntrustedMinion(string ipAddress) {
    return Monitor::untrustedMinions[ipAddress];
}

Minion* Monitor::pickTrustedMinion(){
    if(Monitor::trustedMinions.size() == 0){
        return NULL;
    }

    std::map<string, Minion*>::iterator it = Monitor::trustedMinions.begin();
    std::advance(it, General::random_0_to_n(0, Monitor::trustedMinions.size()));
    return it->second;
}

vector<Minion*> Monitor::pickNTrustedMinions(int numberOfMinions){
    if (Monitor::trustedMinions.size() < numberOfMinions) {
        throw 10; //TODO check if it makes sense to send an exception or return something;
    }

    vector<Minion*> result;
    std::map<string, Minion*>::iterator it = Monitor::trustedMinions.begin();
    for (int i = 0; i < numberOfMinions; i++){
        result.insert(result.end(), it->second);

        ++it;
    }

   return result;
}

//Functions
void Monitor::addApplication(string appID, vector<Minion*> hosts){
    map<string, Application*>::iterator contains = Monitor::applications.find(appID);
    //If contains != Monitor::applications.end it means element found
    if (contains != Monitor::applications.end()){
        throw 10;  //TODO check if it makes sense to send an exception or return something;
    }

    Application* app = new Application(appID);
    Monitor::applications.insert(pair<string, Application*>(appID, app)); 

    vector<Minion*> minionsList;
    std::for_each(hosts.begin(), hosts.end(),
        [app, &minionsList](Minion* minion)
        {
            minion->addApp(app);
            minionsList.insert(minionsList.end(), minion);
            cout << "Added host: " + minion->getIpAddress() + " with app id: " + app->getAppID();
        }
    );
    Monitor::appsHosts.insert(pair<string, vector<Minion*>>(appID, minionsList));
}

void Monitor::deleteApplication(string appID) {
    map<string, Application*>::iterator contains = Monitor::applications.find(appID);
    //If contains == Monitor::applications.end it means element not found
    if (contains == Monitor::applications.end()) {
        throw 10;  //TODO check if it makes sense to send an exception or return something;
    }

    vector<Minion*> toDelete = Monitor::appsHosts[appID];
    std::for_each(toDelete.begin(), toDelete.end(),
                  [appID](Minion* minion) {
                      minion->removeApp(appID);
                  });
    Monitor::applications.erase(appID);
}

void Monitor::addNewMinion(string newMinionAddress) {
    Minion* newMinion = new Minion(newMinionAddress);
    Monitor::trustedMinions.insert(pair<string, Minion*>(newMinion->getIpAddress(), newMinion));
    cout << "Added: " << newMinion->getIpAddress() << " #Minions: " << Monitor::trustedMinions.size() << " \n";
}

void Monitor::removeMinion(string minionAddress) {
    map<string, Minion*>::iterator trustedContains = Monitor::trustedMinions.find(minionAddress);
    map<string, Minion*>::iterator untrustedContains = Monitor::untrustedMinions.find(minionAddress);
    //If contains == Monitor::applications.end it means element not found
    if (trustedContains == Monitor::trustedMinions.end() && untrustedContains == Monitor::untrustedMinions.end()) {
        throw 10;  //TODO check if it makes sense to send an exception or return something;
    }

    Monitor::trustedMinions.erase(minionAddress);
    Monitor::untrustedMinions.erase(minionAddress);
}

void Monitor::setMinionUntrusted(string minionAddress){
    map<string, Minion*>::iterator trustedContains = Monitor::trustedMinions.find(minionAddress);
    map<string, Minion*>::iterator untrustedContains = Monitor::untrustedMinions.find(minionAddress);
    //If contains == Monitor::applications.end it means element not found
    if (trustedContains == Monitor::trustedMinions.end() && untrustedContains == Monitor::untrustedMinions.end()) {
        throw 10;  //TODO check if it makes sense to send an exception or return something;
    }

    //TODO
}

void Monitor::setMinionTrusted(string minionAddress) {
    map<string, Minion*>::iterator untrustedContains = Monitor::untrustedMinions.find(minionAddress);
    //If contains == Monitor::applications.end it means element not found
    if (untrustedContains == Monitor::untrustedMinions.end()) {
        throw 10;  //TODO check if it makes sense to send an exception or return something;
    }

    Minion* minion = untrustedContains->second;
    Monitor::untrustedMinions.erase(minionAddress);
    Monitor::trustedMinions.insert(pair<string, Minion*>(minionAddress, minion));
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

//    std::string s = "Monitor I";
    Monitor* monitor = new Monitor(username, sshKey, hostname);

    AuditorRequestHandler auditorRequestHandler = AuditorRequestHandler(monitor);
    std::thread auditorRequestHandlerThread(AuditorRequestHandler::startAuditorRequestHandler, auditorRequestHandler);

    while (monitor->getApprovedConfiguration() == (unsigned char*)General::NULL_VALUE) {
        std::cout << "Im in the loop \n";
        std::cout << "Waiting for approved config. Current Config -> " << +monitor->getApprovedConfiguration() << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    AHubRequestHandler ahubRequestHandler = AHubRequestHandler(monitor);
    std::thread ahubRequestHandlerThread(AHubRequestHandler::startAHubRequestHandler, ahubRequestHandler);

    DeveloperRequestHandler developerRequestHandler = DeveloperRequestHandler(monitor);
    std::thread developerRequestHandlerThread(DeveloperRequestHandler::startDeveloperRequestHandler, developerRequestHandler);

    MinionRequestHandler minionRequestHandler = MinionRequestHandler(monitor);
    std::thread minionRequestHandlerThread(MinionRequestHandler::startMinionRequestHandler, minionRequestHandler);

    std::cout << "Approved configuration on " << monitor->getHostname() << "\n";

    ahubRequestHandlerThread.join();
    auditorRequestHandlerThread.join();
    developerRequestHandlerThread.join();
    minionRequestHandlerThread.join();

    std::cout << "Bubye\n";

    return 0;
}