#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <iostream>
#include <map>
#include <thread>
#include <vector>
#include "../../Utilities/AttestationConstants.h"
#include "../../Utilities/General.h"
#include "../../Utilities/Messages.h"
#include "../../Utilities/Ports.h"
#include "../Application/Application.h"
#include "Minion.h"

using namespace std;

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
    map<string, Minion*> getUntrustedMinions();
    map<string, Minion*> getTrustedMinions();

    unsigned char* getApprovedConfiguration();
    string getApprovedSHA1();
    string getApprovedConfigurationForMinions();

    //Setters
    void setApprovedConfiguration(unsigned char* approvedConfiguration, string pcrSHA1);
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