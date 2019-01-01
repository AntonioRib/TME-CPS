#ifndef _monitor_h_
#define _monitor_h_
using namespace std;

#include "../Minion/Minion.h"
#include <iostream>
#include <thread>
#include <map>

class Monitor {
    map<string, Minion*> trustedMinions;
    map<string, Minion*> untrustedMinions;
    map<string, string> applications;
    

   private:
    unsigned char* approvedConfiguration;
    ;

   protected:
    ;

   public:
    void setApprovedConfiguration(unsigned char* approvedConfiguration);
    unsigned char* getApprovedConfiguration();
    Monitor();
    Monitor(string username, string sshKey, string hostName);
    Monitor(const Monitor&);
    string username;
    string sshKey;
    string hostname;
};

#endif