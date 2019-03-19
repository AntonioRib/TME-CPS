#ifndef _AUDITINGHUB_H_
#define _AUDITINGHUB_H_

#include <iostream>
#include <map>
#include <thread>
#include <utility>
#include <vector>
#include "../../Utilities/AttestationConstants.h"
#include "../../Utilities/General.h"
#include "../../Utilities/Messages.h"
#include "../../Utilities/Ports.h"
using namespace std;

class AuditingHub
{
  private:
    string hubUserName;
    string hubKey;
    string monitorHost;
    string hostname;

    unsigned char *approvedConfiguration;
    string approvedSHA1;

    map<long, thread*> temporaryThreads;
    map<string, thread*> remoteHostThreadMap;

   public:
    AuditingHub();
    AuditingHub(string hubUserName, string hubKey, string monitorHost, string hotsName);
    AuditingHub(const AuditingHub&);

    //Getters
    string getHubUserName();
    string getHubKey();
    string getMonitorHost();
    string getHostname();

    unsigned char* getApprovedConfiguration();
    string getApprovedSHA1();

    //TODO this may need to pass the map by reference &
    map<long, thread*> getTemporaryThreads(); 
    map<string, thread*> getRemoteHostThreadMap(); 

    //Setters
    void
    setApprovedConfiguration(unsigned char *approvedConfiguration);
    void setApprovedSHA1Configuration(string approvedSHA1Configuration);

    //Functions
    bool checkPermissionAndUnqueue(string remoteHost);
    void removeSession(string remoteHost);
};

#endif