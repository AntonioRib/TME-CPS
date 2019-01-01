#ifndef _monitor_h_
#define _monitor_h_
using namespace std;

#include "../Minion/Minion.h"
#include "../Application/Application.h"
#include <iostream>
#include <thread>
#include <map>
#include <vector>

class Monitor {

   private:
    map<string, Minion*> trustedMinions;
    map<string, Minion*> untrustedMinions;
    map<string, Application*> applications;
    map<string, vector<Minion*>> appsHosts;

    string username;
    string sshKey;
    string hostname;

    unsigned char* approvedConfiguration;
    string approvedSHA1;
    string minionsApprovedSHA1;

   protected:
    ;

   public:
    Monitor();
    Monitor(string username, string sshKey, string hostName);
    Monitor(const Monitor&);

    //Getters
    string getSSHKey();
    string getUsername();
    string getHostname();

    vector<Minion*> getHosts(string appID);
    vector<Minion*> getUntrustedMinions();
    vector<Minion*> getTrustedMinions();

    unsigned char* getApprovedConfiguration();
    string getApprovedConfigurationForMinions();

    //Setters
    void setApprovedConfiguration(unsigned char* approvedConfiguration);
    void setApprovedConfigurationForMinions(string approvedConfiguration);

    //Special Getters
    Minion* getUntrustedMinion(string ipAddress);
    Minion* pickTrustedMinion();
    vector<Minion*> pickNTrustedMinions(int numberOfMinions);

    //Functions
    void addApplication(string appID, vector<Minion*> hosts);
    void deleteApplication(string appID);

    void addNewMinion(string newMinionAddress);
    void removeMinion(string minionAddress);
    void setMinionUntrusted(string minionAddress);
    void setMinionTrusted(string minionAddress);


};

#endif