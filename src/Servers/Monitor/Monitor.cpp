#include "Monitor.h"
#include <chrono>
#include "Handler/AHubRequestHandler.h"
#include "Handler/AuditorRequestHandler.h"
#include "Handler/DeveloperRequestHandler.h"
#include "Handler/MinionRequestHandler.h"
using namespace std;

const int USERNAME_FLAG_INDEX = 1;
const int KEY_FLAG_INDEX = 3;
const int HOSTNAME_FLAG_INDEX = 5;

const unsigned char* NULL_VALUE = (unsigned char*)"\0";

//Constructors
Monitor::Monitor(const Monitor& m) : username{m.username}, sshKey{m.sshKey}, hostname{m.hostname}, 
    trustedMinions{m.trustedMinions}, untrustedMinions{m.untrustedMinions}, applications{m.applications} {
    Monitor::approvedConfiguration = (unsigned char*)NULL_VALUE;  //std::nullptr_t();
    // std::cout << "Monitor created with the name " << Monitor::hostname << " - copy constructor \n";
}

Monitor::Monitor(string username, string sshKey, string hostName) : username{username}, sshKey{sshKey}, hostname{hostName} {
    std::cout << "Monitor created with the name " << Monitor::username << " - string constructor \n";
    Monitor::approvedConfiguration = (unsigned char*)NULL_VALUE;
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

vector<Minion*> Monitor::getHosts(string appID) {
    //TODO
}

vector<Minion*> Monitor::getUntrustedMinions(){
    //TODO
}

vector<Minion*> Monitor::getTrustedMinions() {
    //TODO
}

unsigned char* Monitor::getApprovedConfiguration() {
    return Monitor::approvedConfiguration;
}

string Monitor::getApprovedConfigurationForMinions() {
    //TODO
}

//Setters
void Monitor::setApprovedConfiguration(unsigned char* approvedConfiguration) {
    std::cout << "Updated confg from: " << +Monitor::approvedConfiguration;
    Monitor::approvedConfiguration = approvedConfiguration;
    std::cout << " to: " << +Monitor::approvedConfiguration << "\n";
}

void Monitor::setApprovedConfigurationForMinions(string approvedConfiguration){
    //TODO
}

//Special Getters
Minion* Monitor::getUntrustedMinion(string ipAddress) {
    //TODO
}

Minion* Monitor::pickTrustedMinion(){
    //TODO
}
vector<Minion*> Monitor::pickNTrustedMinions(int numberOfMinions){
    //TODO
}

//Functions
void Monitor::addApplication(string appID, vector<Minion*> hosts){
    //TODO
}

void Monitor::deleteApplication(string appID) {
    //TODO
}

void Monitor::addNewMinion(string newMinionAddress) {
    //TODO
}

void Monitor::removeMinion(string minionAddress) {
    //TODO
}

void Monitor::setMinionUntrusted(string minionAddress){
    //TODO
}

void Monitor::setMinionTrusted(string minionAddress) {
    //TODO
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
    Monitor* monitor = new Monitor(username, sshKey, hostname);

    AHubRequestHandler ahubRequestHandler = AHubRequestHandler(monitor);
    std::thread ahubRequestHandlerThread(AHubRequestHandler::startAHubRequestHandler, ahubRequestHandler);

    AuditorRequestHandler auditorRequestHandler = AuditorRequestHandler(monitor);
    std::thread auditorRequestHandlerThread(AuditorRequestHandler::startAuditorRequestHandler, auditorRequestHandler);

    DeveloperRequestHandler developerRequestHandler = DeveloperRequestHandler(monitor);
    std::thread developerRequestHandlerThread(DeveloperRequestHandler::startDeveloperRequestHandler, developerRequestHandler);

    MinionRequestHandler minionRequestHandler = MinionRequestHandler(monitor);
    std::thread minionRequestHandlerThread(MinionRequestHandler::startMinionRequestHandler, minionRequestHandler);

    while (monitor->getApprovedConfiguration() == (unsigned char*)NULL_VALUE) {
        std::cout << "Im in the loop \n";
        printf("This prints this: %s", monitor->getApprovedConfiguration());
        std::cout << "Waiting for approved config. Current Config -> " << +monitor->getApprovedConfiguration() << "\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    std::cout << "Approved configuration" << +monitor->getApprovedConfiguration() << "\n";

    ahubRequestHandlerThread.join();
    auditorRequestHandlerThread.join();
    developerRequestHandlerThread.join();
    minionRequestHandlerThread.join();

    std::cout << "Bubye\n";

    return 0;
}